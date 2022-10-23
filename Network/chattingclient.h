#ifndef CHATTINGCLIENT_H
#define CHATTINGCLIENT_H

#include "Network/message.h"
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>
#include <thread>
class QPushButton;
class QFile;
class QProgressDialog;

class ChattingClient : public QWidget
{
    Q_OBJECT
public:
    explicit ChattingClient(QWidget *parent = nullptr);

private slots:
    void sendMessage();
    void receiveData();
    void disconnect();
    void logOut();
    void prepareToSendFile();
    void writeFileSlot(FileMessage);
private:
    void initUI();
    QTextEdit* message;
    QLineEdit* inputLine;
    QTcpSocket* clientSocket;
    QLineEdit* idEdit;
    QPushButton* fileButton;
    QPushButton* sentButton;
    QPushButton* loginButton;
    QPushButton* logOutButton;
    void closeEvent(QCloseEvent*) override;
    struct Data {
        bool is_ready=false;
        QByteArray data;
        quint64 target_size=0;
    };
    QHash<QTcpSocket*, Data> socket_data;
    void processMessage(ReadMessage );
    void sendFile(QFile* file, QProgressDialog* progressDialog);
signals:
    void writeFileSignal(FileMessage);
};

#endif // CHATTINGCLIENT_H
