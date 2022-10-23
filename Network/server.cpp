#include "server.h"
#include<QtGui>
#include<QtWidgets>
#include<QtNetwork>
#include "mainwindow.h"
#include <algorithm>
#include <cassert>
using namespace std;

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

}
void Server::clientConnect(){
    qDebug()<<"connect attempt";
    QTcpSocket* clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), SLOT(clientDisconnected()));
    connect(clientConnection,SIGNAL(readyRead()),SLOT(readData()));
    qDebug()<<QString("new connection is established");
}
void Server::clientDisconnected(){
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    assert(socket!=nullptr);
    qDebug()<<socket<<QString("connection disconnected");
}

void Server::readData(){    
    QTcpSocket* socket = (QTcpSocket*)(sender());
    Data& recv_data = socket_data[socket];
    qDebug()<<"ready?"<<recv_data.is_ready;
    if(!recv_data.is_ready){
        if(socket->bytesAvailable()<sizeof(quint64)){
            qDebug()<<"bytes avail"<<socket->bytesAvailable();
            return;
        }
        QDataStream stream{socket};
        stream>>recv_data.target_size ;
        qDebug()<<"size set"<<recv_data.target_size;
        recv_data.is_ready=true;
    }

    recv_data.data.append(socket->read(recv_data.target_size));
    if(recv_data.data.size()<recv_data.target_size){
        qDebug()<<QString("downlading(%1/%2)").arg(recv_data.data.size()).arg(recv_data.target_size);
        return;
    }
//    ReadMessage read_msg {recv_data.data};

    qDebug()<<"read datasssss";

    mgr.processMessage(socket,recv_data.data);

    assert(socket_data.remove(socket));
}


void Server::sendMessage(const QTcpSocket * socket_, const Message& msg){
    auto socket = const_cast<QTcpSocket*>(socket_);
    socket->write(msg);
}
void Server::disconnectSocket(const QTcpSocket* socket_){
    auto socket = const_cast<QTcpSocket*>(socket_);
    socket->close();
}

Server::~Server()
{
}

