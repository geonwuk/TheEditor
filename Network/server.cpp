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
    if(!tcpServer->listen(QHostAddress::Any, PORT_NUMBER)){
        QMessageBox::critical(this,tr("Echo Server"),tr("Unable to start the server: %1")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }
    qDebug()<<QString("The server is running on port %1").arg(tcpServer->serverPort());

}
void Server::clientConnect(){
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
    if(!recv_data.is_ready){
        if(socket->bytesAvailable()<sizeof(quint64)){
            return;
        }
        QDataStream stream{socket};
        stream>>recv_data.target_size ;
        recv_data.is_ready=true;
    }

    recv_data.data.append(socket->read(recv_data.target_size));
    if((quint64)recv_data.data.size()<recv_data.target_size){
        sendMessage(socket,Message{QString::number(recv_data.data.size()),Chat_FileTransmission});      //파일 전송 progress_dialog의 값을 업데이트 하도록 출력
        return;
    }

    mgr.processMessage(socket,recv_data.data);

    socket_data.remove(socket);
}


void Server::sendMessage(const QTcpSocket * socket_, const Message& msg){
    auto socket = const_cast<QTcpSocket*>(socket_);
    socket->write(msg);
    socket->flush();

}
void Server::disconnectSocket(const QTcpSocket* socket_){
    auto socket = const_cast<QTcpSocket*>(socket_);
    socket->close();
}

Server::~Server()
{
}

