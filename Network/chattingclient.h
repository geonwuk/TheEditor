#ifndef CHATTINGCLIENT_H
#define CHATTINGCLIENT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>


class ChattingClient : public QWidget
{
    Q_OBJECT
public:
    explicit ChattingClient(QWidget *parent = nullptr);

private slots:
    void echoData();
    void sendData();
private:
    QTextEdit* message;
    QLineEdit* inputLine;
    QTcpSocket* clientSocket;

signals:

};

#endif // CHATTINGCLIENT_H
