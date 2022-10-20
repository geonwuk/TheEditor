#include "server.h"
#include<QtGui>
#include<QtWidgets>
#include<QtNetwork>
#include "mainwindow.h"
#include <algorithm>
#include <cassert>
#include "View/showview.h"
using namespace std;



void ServerManager::login(QTcpSocket* socket, decltype(net_clients)::iterator itr){
    ip_to_nclient.insert(socket, &(*(itr->second)));
    (itr->second)->is_online=true;
    (itr->second)->socket=socket;
    qDebug()<<"login complete";
    auto client = socketToNetClient(socket);
    if(client==nullptr){
        qDebug()<<"EXEPT";
    }
    assert(nullptr!=client);
    qDebug()<<"client"<<client->self->getId().c_str()<<    client->socket->peerAddress().toString();
    for(auto v : chat_views){
        ;
    }
}

void ServerManager::addLog(QTcpSocket* socket, QString data){
    auto client = socketToNetClient(socket);
    assert(nullptr!=client);
    qDebug()<<"client"<<client->self->getId().c_str()<<    client->socket->peerAddress().toString();
    for(auto v: chat_views){
        v->addLog(client, data);
    }
}

ServerManager::ServerManager(Manager& mgr): mgr{mgr} {
//    for(const auto& client : mgr.getCM().getCleints()){
//        auto s_client = mgr.getCM().copyClient(client.getId());
//        net_clients.emplace_back(s_client);
//    }
}


void ServerManager::unregisterChatView(ShowChatView* view){
    auto it = std::find_if(chat_views.begin(),chat_views.end(),[=](ShowChatView* v){
        return view==v;
    });
    assert(it!=chat_views.end());
    chat_views.erase(it);
}


Server::Server(ServerManager& mgr, QWidget *parent)
    : QWidget(parent), mgr{mgr}
{
    tcpServer= new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),SLOT(clientConnect()));
    if(!tcpServer->listen()){
        QMessageBox::critical(this,tr("Echo Server"),tr("Unable to start the server: %1")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }
    qDebug()<<QString("The server is running on port %1").arg(tcpServer->serverPort());
    setWindowTitle(tr("Echo Server"));
}
void Server::clientConnect(){
    qDebug()<<"connect attempt";
    QTcpSocket* clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    connect(clientConnection,SIGNAL(readyRead()),SLOT(readData()));
    qDebug()<<QString("new connection is established");
}

void Server::readData(){    
    QTcpSocket* socket = (QTcpSocket*)(sender());
    Data& recv_data = socke_data[socket];
    qDebug()<<"ready?"<<recv_data.is_ready;
    if(!recv_data.is_ready){
        if(socket->bytesAvailable()<sizeof(quint64)){
            qDebug()<<"bytes avail"<<socket->bytesAvailable();
            return;
        }
        QDataStream x{socket};

        x>>recv_data.target_size ;
        qDebug()<<"size set"<<recv_data.target_size;
        recv_data.is_ready=true;
    }

    recv_data.data.append(socket->read(recv_data.target_size));
    if(recv_data.data.size()<recv_data.target_size){
        return;
    }
    ReadMessage read_msg {recv_data.data};

    qDebug()<<"read datasssss";

    switch(read_msg) {
    case Chat_Login : {
        qDebug()<<"size"<<recv_data.target_size-1;
        QString id{read_msg.toQString()};
        assert(!id.isEmpty());
        qDebug()<<"ID!!"<<id;
        auto it = mgr.findNetClient(id);
        if(it==mgr.noClient()){
            qDebug()<<"NO ID";
            Message msg {"NO_ID",NO_ID};
            socket->write(msg);
        }
        else{
            Message msg {"SUCCESS",SUCCESS};
            socket->write(msg);
            qDebug()<<"login"<< id;
            mgr.login(socket, it);
        }
        break;
    }
    case Chat_Talk:{
        auto client = mgr.socketToNetClient(socket);
        if(client==nullptr){
            socket->write(Message {"BAD_REQUEST",BAD_REQUEST});
            break;
        }
        qDebug()<<client->self->getId().c_str()<<client->self->getName().c_str();
        qDebug()<<read_msg.toQString();
        QString ip = socket->peerAddress().toString();

        mgr.addLog(socket, read_msg.toQString());

        break;
    }

    }


    assert(socke_data.remove(socket));
}

Server::~Server()
{
}

