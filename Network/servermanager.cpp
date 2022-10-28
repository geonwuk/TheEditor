#include "servermanager.h"
#include "qdebug.h"
#include "View/showview.h"
#include <QTcpSocket>
#include "Network/server.h"
#include <QDateTime>
#include <QFile>

static QList<QString> parseMessage(const QByteArray& data){
    QString info{data};
    QList<QString> ls = info.split(',');
    assert(ls.size()==2); //디버깅 용도로 프로토콜은 (채팅방 번호,메시지 내용)인지 확인하는 용도
    return ls;
}


ServerManager::ServerManager(Manager& mgr): mgr{mgr} {}

NetClient& ServerManager::addClient(const std::shared_ptr<Client>& c)
{
    qDebug()<<"ADDING NEW NC" <<c->getId().c_str();
    auto nc = std::make_shared<NetClient>(c);
    auto result = net_clients.emplace(c->getId(), nc);
    //assert(result.second);
    return *result.first->second;
}

void ServerManager::login(const QTcpSocket* const socket, const QString& id){
    auto nc_itr = net_clients.find(id.toStdString());
    if(nc_itr==net_clients.end()){
        server->sendMessage(socket,Message{0,"NO_ID",Chat_Login});
        return;
    }
    auto nc = nc_itr->second.get();
    nc->is_online=true;
    nc->socket=socket;
    socket_to_nclient.insert(socket, nc);
    qDebug()<<nc->self->getId().c_str()<<"login complete";
    notify();
    server->sendMessage(socket,Message{0,"SUCCESS",Chat_Login});

}

void ServerManager::logOut(const QTcpSocket* const socket){
    if(socket==nullptr)
        return;
    auto itr = socket_to_nclient.find(socket);
    if(itr==socket_to_nclient.end()){
        server->sendMessage(socket, Message{0,"BAD_REQUEST",Chat_LogOut});
        return;
    }
    auto nc = itr.value();
    nc->is_online=false;
    nc->socket=nullptr;
    socket_to_nclient.erase(itr);
    qDebug()<<"log out" <<nc->self->getId().c_str()<<nc->self->getName().c_str();
    notify();
}

void ServerManager::dropClient(const unsigned int chat_room_no_, const QString id){
    auto it = net_clients.find(id.toStdString());
    assert(it!=net_clients.end());
    auto nc = it->second;
    auto socket = nc->socket;
    if(it->second->isOnline()){
        server->sendMessage(socket, Message{chat_room_no_, "",Chat_KickOut});
    }
    else{
       // 오프라인이면
    }
    qDebug()<<chat_room_no_;
    auto chat_room = std::find(chat_rooms.begin(),chat_rooms.end(),chat_room_no_);
    assert(chat_room!=chat_rooms.end());
    auto& participants = chat_room->participants;
    for(auto p = participants.begin(); p!=participants.end(); p++ ){
        if((*p)->self->getId()==id.toStdString()){
            participants.erase(p);
            break;
        }
    }
    nc->removeRoom(chat_room_no_);

    if(it->second->attending_rooms.empty()){
        socket_to_nclient.remove(socket);
        net_clients.erase(it);
    }
    notify();
}

void ServerManager::chatTalk(const QTcpSocket * const socket, const QByteArray& data){
    auto ls = parseMessage(data);
    unsigned int chat_room_no = ls.at(0).toInt();
    auto itr = socket_to_nclient.find(socket);
    if(itr==socket_to_nclient.end()){
        server->sendMessage(socket, Message{chat_room_no, "BAD_REQUEST", Chat_Talk});
        return;
    }
    auto nc = itr.value();

    QString ip = socket->peerAddress().toString();
    QString port = QString::number(socket->peerPort());
    QString id = nc->self->getId().c_str();
    QString name = nc->self->getName().c_str();
    QString chat = ls.at(1);
    QString time = QDateTime::currentDateTime().toString();
    const ChatMessage msg{ip,port,id,name,chat,time};
    for(auto itr : net_clients){
        auto peer_socket = itr.second->socket;
        if(peer_socket!=socket){
            QString str = id + ',' + chat;
            server->sendMessage(peer_socket,Message(chat_room_no,str,Chat_Talk));
        }
    }
    auto chat_room = std::find(chat_rooms.begin(),chat_rooms.end(),chat_room_no);
    assert(chat_room!=chat_rooms.end());
    chat_room->logs.emplace_back(msg);
    for(auto v: chat_views){
        v->addLog(msg);
    }
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

void ServerManager::fileTransmission(const QTcpSocket* const socket, QByteArray& data){
    auto nc = socket_to_nclient[socket];
    qDebug()<<"File transmission to server writing";
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
    qDebug()<<"file_name_tmp"<<str;
    QString name {QString("%1_%2").arg(nc->self->getId().c_str()).arg(str)};
    QFile x{name};
    x.open(QIODeviceBase::WriteOnly);
    qDebug()<<"filesize"<<data.size();
    x.write(data);

}

bool ServerManager::attendClient(unsigned int chat_room_no_, NetClient& nc){
    qDebug()<<"chat room no"<<chat_room_no_<<nc.self->getId().c_str();
    auto chat_room = std::find(chat_rooms.begin(),chat_rooms.end(),chat_room_no_);
    chat_room->participants.emplace_back(&nc);
    nc.attending_rooms.emplace_back(&*chat_room);
    return true;
}

void ServerManager::notify(){
    for(auto v : chat_views){
        v->update();
    }
}

