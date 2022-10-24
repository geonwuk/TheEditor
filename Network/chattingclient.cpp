#include "Network/chattingclient.h"
#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>
#include "Network/message.h"
#include <QFileInfo>
using namespace std;
void ChattingClient::initUI(){
    QLineEdit* serverAddress = new QLineEdit(this);
    serverAddress->setText("127.0.0.1");
    QRegularExpression re{"^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|"};
    QRegularExpressionValidator validator(re);
    serverAddress->setPlaceholderText("Server IP Address");

    QLineEdit* serverPort = new QLineEdit(this);
    serverPort->setInputMask("00000;_");
    serverPort->setPlaceholderText("Server Port No");

    idEdit = new QLineEdit(this);
    idEdit->setPlaceholderText("ID");

    loginButton = new QPushButton("login",this);
    connect(loginButton,&QPushButton::clicked,[=](){
        clientSocket->connectToHost(serverAddress->text(),
                                    serverPort->text().toInt());
        Message msg{idEdit->text(), Chat_Login};
        clientSocket->write(msg);
    });

    QHBoxLayout* serverLayout = new QHBoxLayout;
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
    serverLayout->addWidget(idEdit);
    serverLayout->addWidget(loginButton);

    message=new QTextEdit(this);
    message->setReadOnly(true);

    inputLine = new QLineEdit(this);
    connect(inputLine, SIGNAL(returnPressed( )), SLOT(sendMessage()));
    connect(inputLine, SIGNAL(returnPressed( )), inputLine, SLOT(clear( )));
    sentButton = new QPushButton("Send",this);
    connect(sentButton,SIGNAL(clicked()),SLOT(sendMessage()));
    connect(sentButton, SIGNAL(clicked( )), inputLine, SLOT(clear( )));
    //inputLine->setDisabled(true);
    //sentButton->setDisabled(true);

    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sentButton);

    fileButton = new QPushButton("File Transfer", this);
    connect(fileButton, SIGNAL(clicked()), SLOT(prepareToSendFile()));
    fileButton->setDisabled(true);

    logOutButton = new QPushButton("Log Out", this);
    logOutButton->setDisabled(true);
    connect(logOutButton, SIGNAL(pressed()),SLOT(logOut()));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(fileButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(logOutButton);


    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(serverLayout);
    mainLayout->addWidget(message);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    inputLine->setEnabled(false);
    idEdit->setReadOnly(false);
    sentButton->setEnabled(false);


}

ChattingClient::ChattingClient(QWidget *parent)
    : QWidget{parent}
{
    initUI();

    clientSocket = new QTcpSocket(this);
    connect(clientSocket,&QAbstractSocket::errorOccurred,
            [=]{qDebug()<<clientSocket->errorString();});
     connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));
     connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));


    setWindowTitle(tr("Client"));
}



void ChattingClient::sendMessage(){
    QString str = inputLine->text( );
    if(str.length( )) {
        message->append("<font color=red>나</font> : " + str);
        Message msg{str,Chat_Talk};
        clientSocket->write(msg);
    }
}

void ChattingClient::receiveData(){
    QTcpSocket* socket = (QTcpSocket*)(sender());
    Data& recv_data = socket_data[socket];
    qDebug()<<"ready?"<<recv_data.is_ready;
    if(!recv_data.is_ready){
        if(socket->bytesAvailable()<sizeof(quint64)){
            qDebug()<<"bytes avail"<<socket->bytesAvailable();
            return;
        }
        QDataStream stream{socket};
        stream>>recv_data.target_size;
        qDebug()<<"size set"<<recv_data.target_size;
        recv_data.is_ready=true;
    }

    recv_data.data.append(socket->read(recv_data.target_size));
    if(recv_data.data.size()<recv_data.target_size){
        return;
    }
    ReadMessage read_msg {recv_data.data};
    processMessage(read_msg);
    assert(socket_data.remove(socket));
}

void ChattingClient::closeEvent(QCloseEvent*)
{
    logOut();
}

void ChattingClient::disconnect( )
{
    QMessageBox::critical(this, tr("Chatting Client"), tr("Disconnect from Server"));
    inputLine->setEnabled(false);
    idEdit->setReadOnly(false);
    sentButton->setEnabled(false);
    loginButton->setText(tr("Login"));
}

void ChattingClient::processMessage(ReadMessage rmsg){
    if(rmsg.toQString()=="BAD_REQUEST"){
        qDebug()<<"bad request recived";
        return;
    }
    QString str=rmsg.toQString();
    switch(rmsg){
    case Chat_Login:
        if(str=="NO_ID"){
            QMessageBox::critical(this, tr("Chatting Client"), tr("No such ID %1 exists").arg(idEdit->text()));
        }
        else{
            loginButton->setEnabled(false);
            logOutButton->setEnabled(true);
            inputLine->setEnabled(true);
            sentButton->setEnabled(true);
            fileButton->setEnabled(true);
        }
        break;
    case Chat_Talk: {
        auto ls = str.split(',');
        QString id = ls[0];
        str.remove(0,id.size()+1);
        message->append(QString("<font color=blue>%1</font> : ").arg(id)+str);
        break;
    }
    case Chat_KickOut:
        QMessageBox::critical(this, tr("Chatting Client"), tr("Kick out from Server"));
        inputLine->setDisabled(true);
        sentButton->setDisabled(true);
        fileButton->setDisabled(true);
        logOutButton->setDisabled(true);
        idEdit->setReadOnly(false);
        loginButton->setEnabled(true);
        break;
    }
}

void ChattingClient::logOut(){
    clientSocket->write(Message{"",Chat_LogOut});
    clientSocket->disconnectFromHost();
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();
    inputLine->setDisabled(true);
    sentButton->setDisabled(true);
    fileButton->setDisabled(true);
    logOutButton->setDisabled(true);
    loginButton->setEnabled(true);
    idEdit->setReadOnly(false);
}

void ChattingClient::sendFile(QFile* file, QProgressDialog* progressDialog){
    try{
        emit writeFileSignal(FileMessage {file, progressDialog});
    }
    catch(...){
        QMessageBox::critical(this, tr("ERROR"), tr("Error while uploading file %1").arg(file->fileName()));
        delete file;
        delete progressDialog;
    }
    qDebug()<<"send complete!!!!!!!!!!";
}

void ChattingClient::prepareToSendFile(){
    QString filename = QFileDialog::getOpenFileName(this,tr("Upload file"));
    if(filename.length()) {
        QFile* file = new QFile(filename, this);
        QProgressDialog* progressDialog = new QProgressDialog(QString(tr("Uploading file %1").arg(filename)),tr("&Cancel"),0,file->size()*2,this );
        file = new QFile(filename);
        file->open(QFile::ReadOnly);

        qDebug() << QString("file %1 is opened").arg(filename);
        writeFileSlot(FileMessage {file, progressDialog});
//        std::thread t1{ &ChattingClient::sendFile, this, file, progressDialog};
//        connect(this,SIGNAL(writeFileSignal(FileMessage)),SLOT(writeFileSlot(FileMessage)));
//        t1.detach();
    }
}

void ChattingClient::writeFileSlot(FileMessage file_msg){
    qDebug()<<"writeFileSlot";
    qint64 bytes_written = clientSocket->write(file_msg);
    file_msg.updateProgress(bytes_written);
}
