#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QList>

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();
private slots:
    void clientConnect();
    void echoData();
private:
    QLabel *infoLabel;
    QTcpServer* tcpServer;
    QList<QTcpSocket*> clientList;
};


#endif // SERVER_H
