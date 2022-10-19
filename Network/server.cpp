#include "server.h"
#include<QtGui>
#include<QtWidgets>
#include<QtNetwork>
#include "mainwindow.h"
#include <algorithm>
#include <cassert>
#include "View/showview.h"
using namespace std;

ServerManager::ServerManager(Manager& mgr): mgr{mgr} {
//    for(const auto& client : mgr.getCM().getCleints()){
//        auto s_client = mgr.getCM().copyClient(client.getId());
//        net_clients.emplace_back(s_client);
//    }
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

    QDataStream in(&recv_data.data, QIODevice::ReadOnly);
    char type;
    in >> type;

    qDebug()<<"read datasssss";




    switch(type) {
    case Chat_Login : {
        qDebug()<<"size"<<recv_data.target_size-1;
        char* x = new char[recv_data.target_size];
        x[recv_data.target_size-1]='\0';
        in.readRawData(x,recv_data.target_size-1);

        for(int i=0; i<recv_data.target_size-1; i++){
            qDebug()<<x[i];
        }
        QString id{x};
        qDebug()<<"ID!!"<<id;
        auto it = mgr.findNetClient(id);
        if(it==mgr.end()){
            qDebug()<<"NO ID";
            Message msg {"",NO_ID};
            socket->write(msg);
        }
        else{
            Message msg {"",SUCCESS};
            socket->write(msg);
            qDebug()<<"login"<< id;
            it->is_online=true;
            ip_to_nclient.insert(socket, &*it);
        }
        delete[] x;
        break;
    }
    case Chat_Talk:{
        QString ip = socket->peerAddress().toString();
        auto client = ip_to_nclient[socket];
        qDebug()<<client->self->getId().c_str()<<client->self->getName().c_str();
        char* x = new char[recv_data.target_size];
        x[recv_data.target_size-1]='\0';
        in.readRawData(x,recv_data.target_size-1);
        QString id{x};
        qDebug()<<id;
        delete[] x;

        break;
    }

    }


    assert(socke_data.remove(socket));
}

Server::~Server()
{
}

template<typename F>
void ServerManager::updateView(F f){
    mgr.notify([](View* v){
        ShowChatView* view =dynamic_cast<ShowChatView*>(v);
        if(nullptr!=view){
            qDebug()<<v->label <<"network notify";
        }

    });
}
