#include "servermanager.h"

#include "View/showview.h"
#include <QTcpSocket>
#include "Network/server.h"
#include <QDateTime>
#include <QFile>
ServerManager::ServerManager(Manager& mgr): mgr{mgr} {}
ServerManager::~ServerManager(){
    delete server;
    server = nullptr;
    chat_views.clear();
    socket_to_nclient.clear();
}
void ServerManager::addClient(const Client& c)
{
    assert(net_clients.emplace(make_pair(c.getId(),NetClient{c,0})).second);
}
void ServerManager::login(const QTcpSocket* const socket, const QString& id){
    auto nc_itr = net_clients.find(id.toStdString());                               //로그인할 id로 고객 정보를 찾습니다
    if(nc_itr==net_clients.end()){                                                  //만약 addChatView에서 고객을 채팅 참여자로 설정하지 않았다면 net_clients에 추가가 안되었다는 의미입니다
        server->sendMessage(socket,Message{"NO_ID",Chat_Login});                    //로그인을 시도한 클라이언트에게 해당 ID로는 채팅에 참여할 수 없다고 메시지를 보냅니다
        return;
    }
    auto& nc = nc_itr->second;       //map에서 net_client를 찾습니다
    nc.is_online=true;
    nc.socket=socket;
    socket_to_nclient.insert({socket, &nc});
    notify();                                                           //chat_view만 업데이트
    server->sendMessage(socket,Message{"SUCCESS",Chat_Login});          //로그인 성공 메시지를 클라이언트에게 보냅니다
}
void ServerManager::logOut(const QTcpSocket* const socket){
    if(socket==nullptr)
        return;
    auto itr = socket_to_nclient.find(socket);
    if(itr==socket_to_nclient.end()){
        server->sendMessage(socket, Message{"BAD_REQUEST",Chat_LogOut});
        return;
    }
    auto nc = itr->second;
    nc->is_online=false;
    nc->socket=nullptr;
    socket_to_nclient.erase(itr);
    notify();
}
void ServerManager::dropClient(QString id){
    auto it = net_clients.find(id.toStdString());                   //채팅방 참여자의 id로 고객정보를 찾습니다
    assert(it!=net_clients.end());                                  //채팅방 참여자에서 drop을한다는 것은 이미 net_clients에 id가 있도록 GUI를 구성했으므로 못 찾는 경우는 코드 오류입니다
    auto socket = it->second.socket;
    if(it->second.isOnline()){                                      //온라인이면 메시지 보내기
        server->sendMessage(socket, Message{"",Chat_KickOut});      //강퇴 됐다는 메시지 보내기
    }
    else{
       // 오프라인이면
    }
    socket_to_nclient.erase(socket);                               //강퇴한 클라이언트와 소통하던 소켓을 삭제한다
    net_clients.erase(it);                                          //강퇴한 클라이언트 정보를 삭제하고 채팅 참여자 리스트에서 제외시킨다
    notify();                                                       //
}
void ServerManager::chatTalk(const QTcpSocket * const socket, const QByteArray& data){  //로그인된 클라이언트에서 채팅을 시도하면 서버에서 Chat_Talk를 수신하고 이 함수를 호출합니다
    auto itr = socket_to_nclient.find(socket);  //소켓으로 누구인지 결정합니다
    if(itr==socket_to_nclient.end()){   //net_client에 없다는 의미는 채팅방 참여자가 아닌데 채팅 시도를 한다는 의미이므로
        server->sendMessage(socket, Message{"BAD_REQUEST",Chat_Talk});  //BAD_REQUEST를 보내고 채팅시도를 차단합니다
        return;
    }
    auto nc = itr->second;  //소켓으로 채팅방 참여자 정보(net_client)를 획득합니다

    QString ip = socket->peerAddress().toString();
    QString port = QString::number(socket->peerPort());
    QString id = nc->self.getId().c_str();
    QString name = nc->self.getName().c_str();
    QString chat {data};
    QString time = QDateTime::currentDateTime().toString();
    const ChatMessage msg{ip,port,id,name,chat,time};   //ip,port,id,name,채팅 내용,시각을 담은 메시지를 생성합니다(로그에 기록할 용도)
    for(auto& itr : net_clients){                       //채팅방에 참여한 모든 멤버들에 대해서
        if(itr.second.isOnline()){                      //온라인이면
            auto peer_socket = itr.second.socket;       //소켓을 획득한 후
            if(peer_socket!=socket){                    //채팅 메시지를 보내는 본인을 제외하고 메시지를 보냅니다
                QString str = id + ',' + chat;
                server->sendMessage(peer_socket,Message(str,Chat_Talk));    //채팅 메시지를 송신합니다
            }
        }
    }

    for(auto v: chat_views){    //채팅방 탭들에 대해서 정보를 갱신해줍니다
        v->addLog(msg);
    }
    logs.push_back(msg);        //로그에 추가합니다
    ++log_no;
}
void ServerManager::unregisterChatView(ShowChatView* view){
    auto it = std::find_if(chat_views.begin(),chat_views.end(),[=](ShowChatView* v){
        return view==v;
    });
    assert(it!=chat_views.end());       //GUI 상에서 unregister를 하기 위해서는 register를 먼저 하도록 헀으므로 못 찾는 다는 것은 프로그램 로직 실수 입니다
    chat_views.erase(it);               //
}
void ServerManager::processMessage(const QTcpSocket* const socket, QByteArray data){
    QDataStream in{ &data, QIODevice::ReadOnly};
    char type;
    in.readRawData(&type,1);                //타입 데이터를 읽습니다
    data.remove(0,1);                       //타입 데이터를 수신된 데이터에서 삭제합니다.(위에서 이미 추출했으므로)

    switch(type){                           //수신된 데이터의 타입에 따라 그 타입에 맞는 함수를 호출합니다
    case Chat_Login:{               //로그인인 경우
        QString rmsg {data};        //rmsg는 로그인에 필요한 정보(ID)를 갖습니다
        login(socket, rmsg);}       //로그인 함수를 호출해 채팅 참여자라면 로그인 시키고 아니라면 BAD_REQUEST를 보냅니다
        break;
    case Chat_LogOut:               //로그아웃인 경우
        logOut(socket);             //소켓으로 ID를 알 수 있으므로 소켓만 로그아웃 함수에 입력합니다.
        break;
    case Chat_Talk:{                //채팅 메시지를 송신한 경우
        chatTalk(socket,data);}     //로그인된 다른 채팅방 참여자에게 메시지를 보내야 합니다
        break;
    case Chat_FileTransmission:         //파일 전송인 경우
        fileTransmission(socket,data);  //파일을 서버에 저장합니다
        break;
    }

}
static QString parseString(QByteArray& data){   //파일 전송 프로토콜에서 파일이름과 파일을 보낸 ID
    int i=0;
    QString str;
    for(auto e:data){   //1바이트씩 nullptr을 만날 때까지 반복문을 실행합니다
        if(e!='\0'){    //해당 바이트가 nullptr가 아닌 경우 스트링에 데이터를 추가합니다
            str+=e;     //스트링에 데이터 추가
            ++i;
        }
        else{
            break;
        }
    }
    data.remove(0,i+1);         //nullptr을 포함한 문자열 크기 만큼 데이터에서 삭제해줍니다
    return str;                 //string 데이터를 리턴합니다(파일 이름 또는 ID)
}
void ServerManager::fileTransmission(const QTcpSocket* const socket, QByteArray& data){
    QString file_name_ = parseString(data); //nullptr을 끝으로 파일이름을 추출합니다
    QString sender_id = parseString(data);  //nullptr을 끝으로 보낸사람 ID를 추출합니다
    QString name = QString("%1_%2").arg(sender_id, file_name_); //파일 이름 형식("보낸사람ID_파일이름")
    QFile f{name};
    f.open(QIODeviceBase::WriteOnly);
    f.write(data);
    server->sendMessage(socket,Message{QString::number(data.size()),Chat_FileTransmission}); //파일을 보낸 사람에게 파일 저장이 완료 됐다는 의미의 메시지를 보냅니다
}
void ServerManager::notify(){
    for(auto v : chat_views){   //채팅 메시지 뷰를 업데이트 합니다
        v->update();
    }
}
