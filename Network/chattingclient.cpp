#include "Network/chattingclient.h"
#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>
#include "Network/message.h"
#define BLOCK_SIZE 1024


ChattingClient::ChattingClient(QWidget *parent)
    : QWidget{parent}
{
    QLineEdit* serverAddress = new QLineEdit(this);
    serverAddress->setText("127.0.0.1");
    QRegularExpression re{"^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|"};
    QRegularExpressionValidator validator(re);
    serverAddress->setPlaceholderText("Server IP Address");

    QLineEdit* serverPort = new QLineEdit(this);
    serverPort->setInputMask("00000;_");

    QLineEdit* idEdit = new QLineEdit(this);
    idEdit->setPlaceholderText("ID");

    QPushButton* connectButton = new QPushButton("cnnect",this);
    connect(connectButton,&QPushButton::clicked,[=](){
        clientSocket->connectToHost(serverAddress->text(),
                                    serverPort->text().toInt());
    });

    QHBoxLayout* serverLayout = new QHBoxLayout;
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
    serverLayout->addWidget(idEdit);
    serverLayout->addWidget(connectButton);

    message=new QTextEdit(this);
    message->setReadOnly(true);

    inputLine = new QLineEdit(this);
    QPushButton* sentButton = new QPushButton("Send",this);
    connect(sentButton,SIGNAL(clicked()),SLOT(sendData()));

    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sentButton);

    QPushButton* loginButton = new QPushButton("login",this);


    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(loginButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(serverLayout);
    mainLayout->addWidget(message);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    clientSocket = new QTcpSocket(this);
    connect(clientSocket,&QAbstractSocket::errorOccurred,
            [=]{qDebug()<<clientSocket->errorString();});
   // connect(clientSocket, SIGNAL(readyRead()), SLOT(echoData()));
    connect(loginButton,&QPushButton::pressed, [=](){
        Message msg{idEdit->text(), Chat_Login};
        clientSocket->write(msg);
    });
    setWindowTitle(tr("Echo Client"));

}


void ChattingClient::echoData(){
    QTcpSocket* clientSocket = dynamic_cast<QTcpSocket*>(sender());
//    if(clientSocket->bytesAvailable()>BLOCK_SIZE) return;
//    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
//    message->append(QString(bytearray));
}

void ChattingClient::sendData(){
    QString str = inputLine->text();
    if(str.length()){
        Message msg{str,Chat_Talk};
        clientSocket->write(msg);
    }
}
