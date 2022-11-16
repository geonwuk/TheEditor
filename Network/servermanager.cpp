#include "servermanager.h"
#include "qdebug.h"
#include "View/showview.h"
#include <QTcpSocket>
#include "Network/server.h"
#include <QDateTime>
#include <QFile>
ServerManager::ServerManager(Manager& mgr): mgr{mgr} {}

void ServerManager::addClient(const Client& c)
{
    auto result = net_clients.emplace(make_pair(c.getId(),NetClient{c,0}));
    assert(result.second);
}

void ServerManager::login(const QTcpSocket* const socket, const QString& id){
    auto nc_itr = net_clients.find(id.toStdString());                               //로그인할 id로 고객 정보를 찾습니다
    if(nc_itr==net_clients.end()){                                                  //만약 addChatView에서 고객을 채팅 참여자로 설정하지 않았다면 net_clients에 추가가 안되었다는 의미입니다
        server->sendMessage(socket,Message{"NO_ID",Chat_Login});                    //로그인을 시도한 클라이언트에게 해당 ID로는 채팅에 참여할 수 없다고 메시지를 보냅니다
        return;
    }
    auto nc = nc_itr->second;       //map에서 net_client를 찾습니다
    nc.is_online=true;
    nc.socket=socket;
    socket_to_nclient.insert(socket, &nc);
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
    auto nc = itr.value();
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
    socket_to_nclient.remove(socket);                               //강퇴한 클라이언트와 소통하던 소켓을 삭제한다
    net_clients.erase(it);                                          //강퇴한 클라이언트 정보를 삭제하고 채팅 참여자 리스트에서 제외시킨다
    notify();                                                       //
}

void ServerManager::chatTalk(const QTcpSocket * const socket, const QByteArray& data){
    auto itr = socket_to_nclient.find(socket);
    if(itr==socket_to_nclient.end()){
        server->sendMessage(socket, Message{"BAD_REQUEST",Chat_Talk});
        return;
    }
    auto nc = itr.value();

    QString ip = socket->peerAddress().toString();
    QString port = QString::number(socket->peerPort());
    QString id = nc->self.getId().c_str();
    QString name = nc->self.getName().c_str();
    QString chat {data};
    QString time = QDateTime::currentDateTime().toString();
    const ChatMessage msg{ip,port,id,name,chat,time};
    for(auto& itr : net_clients){
        if(itr.second.isOnline()){
            auto peer_socket = itr.second.socket;
            if(peer_socket!=socket){
                QString str = id + ',' + chat;
                server->sendMessage(peer_socket,Message(str,Chat_Talk));
            }
        }
    }

    for(auto v: chat_views){
        v->addLog(msg);
    }
    logs.push_back(msg);
    ++log_no;
}




void ServerManager::unregisterChatView(ShowChatView* view){
    auto it = std::find_if(chat_views.begin(),chat_views.end(),[=](ShowChatView* v){
        return view==v;
    });
    assert(it!=chat_views.end());
    chat_views.erase(it);
}



void ServerManager::processMessage(const QTcpSocket* const socket, QByteArray data){
    QDataStream in{ &data, QIODevice::ReadOnly};
    char type;
    in.readRawData(&type,1);
    data.remove(0,1);

    switch(type){
    case Chat_Login:{
        QString rmsg {data};
        login(socket, rmsg);}
        break;
    case Chat_LogOut:
        logOut(socket);
        break;
    case Chat_Talk:{
        chatTalk(socket,data);}
        break;
    case Chat_FileTransmission:
        fileTransmission(socket,data);
        break;
    }

}

static QString parseString(QByteArray& data){
    int i=0;
    QString str;
    for(auto e:data){
        if(e!='\0'){
            str+=e;
            ++i;
        }
        else{
            break;
        }
    }
    data.remove(0,i+1);
    return str;
}

void ServerManager::fileTransmission(const QTcpSocket* const socket, QByteArray& data){
    QString file_name_ = parseString(data);

    QString sender_id = parseString(data);
    QString name = QString("%1_%2").arg(sender_id).arg(file_name_);
    QFile f{name};
    f.open(QIODeviceBase::WriteOnly);

    f.write(data);
    server->sendMessage(socket,Message{QString::number(data.size()),Chat_FileTransmission});
}

void ServerManager::notify(){
    for(auto v : chat_views){
        v->update();
    }
}
