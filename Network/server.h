#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QHash>


#define PORT_NUMBER 9718

class QTcpServer;
class QTcpSocket;
class ServerManager;
class Message;
class Server : public QWidget
{
    Q_OBJECT

public:
    Server(ServerManager&, QWidget *parent = nullptr);
    ~Server();
    void sendMessage(const QTcpSocket*, const Message&);
    void disconnectSocket(const QTcpSocket*);
private slots:
    void clientConnect();
    void clientDisconnected();
    void readData();
private:
    QTcpServer* tcpServer;
    struct Data {
        bool is_ready=false;
        QByteArray data;
        quint64 target_size=0;
    };
    QHash<QTcpSocket*, Data> socket_data;
    ServerManager& mgr;
};



#endif // SERVER_H
