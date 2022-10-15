#include "server.h"
#include<QtGui>
#include<QtWidgets>
#include<QtNetwork>

#define BLOCK_SIZE 1024
Server::Server(QWidget *parent)
    : QWidget(parent)
{
    infoLabel = new QLabel(this);
    QPushButton* quitButton = new QPushButton("Quit", this);
    connect(quitButton,SIGNAL(clicked()),qApp,SLOT(quit()));

    QHBoxLayout* buttonlLayout = new QHBoxLayout;
    buttonlLayout->addStretch(1);
    buttonlLayout->addWidget(quitButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(buttonlLayout);
    setLayout(mainLayout);

    tcpServer= new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),SLOT(clientConnect()));
    if(!tcpServer->listen()){
        QMessageBox::critical(this,tr("Echo Server"),tr("Unable to start the server: %1")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }
    infoLabel->setText(tr("The server is running on port %1").arg(tcpServer->serverPort()));
    setWindowTitle(tr("Echo Server"));
}
void Server::clientConnect(){
    QTcpSocket* clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    connect(clientConnection,SIGNAL(readyRead()),SLOT(echoData()));
    infoLabel->setText("new connection is established");
    clientList.append(clientConnection);
}

void Server::echoData(){
    QTcpSocket* clientConnection = (QTcpSocket*)sender();
    if(clientConnection->bytesAvailable()>BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    for(auto sock: clientList){
        if(sock!=clientConnection)
            sock->write(bytearray);
    }
    qDebug()<<bytearray;
    clientConnection->write(bytearray);
    infoLabel->setText(QString(bytearray));
}

Server::~Server()
{
}
