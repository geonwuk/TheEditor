#include "servermanager.h"
#include "qdebug.h"
#include "View/showview.h"
#include <QTcpSocket>
#include "Network/server.h"
#include <QDateTime>
ServerManager::ServerManager(Manager& mgr): mgr{mgr} {}

void ServerManager::addClient(const std::shared_ptr<Client>& c)
{
    qDebug()<<"ADDING NEW NC";
    auto nc = std::make_shared<NetClient>(c, log_no);
    auto result = net_clients.emplace(c->getId(), nc);
    assert(result.second);
}

void ServerManager::login(const QTcpSocket* const socket, const ReadMessage& rmsg){
    const QString id{rmsg.toQString()};
    auto nc_itr = net_clients.find(id.toStdString());
    if(nc_itr==net_clients.end()){
        server->sendMessage(socket,Message{"NO_ID",Chat_Login});
        return;
    }
    auto nc = nc_itr->second.get();
    nc->is_online=true;
    nc->socket=socket;
    socket_to_nclient.insert(socket, nc);
    qDebug()<<nc->self->getId().c_str()<<"login complete";
    notify();
    server->sendMessage(socket,Message{"SUCCESS",Chat_Login});

    //todo
    //현재 로그 no와 클라이언트 로그 no를 비교해서 클라이언트에 쏴주기
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
    qDebug()<<"log out" <<nc->self->getId().c_str()<<nc->self->getName().c_str();
    notify();
}

void ServerManager::dropClient(QString id){
    auto it = net_clients.find(id.toStdString());
    assert(it!=net_clients.end());
    auto socket = it->second->socket;
    server->sendMessage(socket, Message{"",Chat_KickOut});
    socket_to_nclient.remove(socket);
    net_clients.erase(it);
    notify();
}

void ServerManager::chatTalk(const QTcpSocket * const socket, const ReadMessage& rmsg){
    auto itr = socket_to_nclient.find(socket);
    if(itr==socket_to_nclient.end()){
        server->sendMessage(socket, Message{"BAD_REQUEST",Chat_Talk});
        return;
    }
    auto nc = itr.value();

    QString ip = socket->peerAddress().toString();
    QString port = QString::number(socket->peerPort());
    QString id = nc->self->getId().c_str();
    QString name = nc->self->getName().c_str();
    QString chat = rmsg.toQString();
    QString time = QDateTime::currentDateTime().toString();
    const ChatMessage msg{ip,port,id,name,chat,time};
    for(auto itr : net_clients){
        auto peer_socket = itr.second->socket;
        if(peer_socket!=socket){
            QString str = id + ',' + chat;
            server->sendMessage(peer_socket,Message(str,Chat_Talk));
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



void ServerManager::processMessage(const QTcpSocket* const socket, const ReadMessage rmsg){
    switch(rmsg){
    case Chat_Login:
        login(socket, rmsg);
        break;
    case Chat_LogOut:
        logOut(socket);
        break;
    case Chat_Talk:
        chatTalk(socket,rmsg);
        break;
    }

}

void ServerManager::notify(){
    for(auto v : chat_views){
        v->update();
    }
}
