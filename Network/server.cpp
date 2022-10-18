#include "server.h"
#include<QtGui>
#include<QtWidgets>
#include<QtNetwork>
#include "mainwindow.h"
#include <algorithm>

using namespace std;

ServerManager::ServerManager(Manager& mgr): mgr{mgr} {
//    for(const auto& client : mgr.getCM().getCleints()){
//        auto s_client = mgr.getCM().copyClient(client.getId());
//        net_clients.emplace_back(s_client);
//    }
}



#define BLOCK_SIZE 1024
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
    clientList.append(clientConnection);
}

void Server::readData(){
    QTcpSocket* clientConnection = (QTcpSocket*)sender();
    if(clientConnection->bytesAvailable()>BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    chatProtocolType data;
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1023);
    qDebug()<<"read datasssss"<<data.type<<data.data;

    QString ip = clientConnection->peerAddress().toString();
    QString content = QString::fromStdString(data.data);

    switch(data.type) {
    case Chat_Login : {
        auto it = find_if(mgr.net_clients.begin(),mgr.net_clients.end(), [=](NetClient& nc){
            if(content == (nc.self->getId().c_str())){
                return true;
            }
            return false;
        });
        if(it==mgr.net_clients.end()){
            chatProtocolType data_to_write;
            data_to_write.type=RESPOND::NO_ID;
            QByteArray data {(char*)(&data_to_write),sizeof(data_to_write)};
            clientConnection->write(data);
        }
        qDebug()<<"login";

        ipToClient.insert(content,&*it);
        break;
    }
    case Chat_Talk:{
        auto client = ipToClient[ip];
        qDebug()<<client->self->getId().c_str()<<client->self->getName().c_str();
        break;
    }

    }
    for(auto sock: clientList){
        if(sock!=clientConnection)
            sock->write(bytearray);
    }
    qDebug()<<bytearray;
    clientConnection->write(bytearray);
    qDebug()<<QString(bytearray);
}

Server::~Server()
{
}
