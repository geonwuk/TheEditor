#include "Network/chattingclient.h"
#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>
#include "server.h"
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

    QPushButton* connectButton = new QPushButton("cnnect",this);
    connect(connectButton,&QPushButton::clicked,[=](){
        clientSocket->connectToHost(serverAddress->text(),
                                    serverPort->text().toInt());
    });

    QHBoxLayout* serverLayout = new QHBoxLayout;
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
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
    connect(clientSocket, SIGNAL(readyRead()), SLOT(echoData()));
    connect(loginButton,&QPushButton::pressed, [this](){
        chatProtocolType data;
        data.type=Chat_Status::Chat_Login;
        data.data[0]='a';
        data.data[1]='a';

        QByteArray s;
        s.append(data.data).append(data.data);
        clientSocket->write(s);
    });
    setWindowTitle(tr("Echo Client"));

}


void ChattingClient::echoData(){
    QTcpSocket* clientSocket = dynamic_cast<QTcpSocket*>(sender());
    if(clientSocket->bytesAvailable()>BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
    message->append(QString(bytearray));
}

void ChattingClient::sendData(){
    QString str = inputLine->text();
    if(str.length()){
        QByteArray bytearray;
        bytearray=str.toUtf8();

        chatProtocolType data;
        data.type=Chat_Status::Chat_Talk;

        for(int i=0; i<str.size(); i++){
            data.data[i]= *(const char*)(bytearray);
        }


        QByteArray data_to_send;
        data_to_send.setRawData(reinterpret_cast<char*>(&data), 1024);

        clientSocket->write(data_to_send);
    }
}
