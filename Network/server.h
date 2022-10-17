#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QList>
#include <vector>
#include "Manager/ClientManager.h"

using CM::Client;
class ChatRoom;
class QTcpServer;
class QTcpSocket;
class QLabel;
class Manager;
class CommandMessage{
public:
    enum class type {Invitation,Drop};

};

struct NetClient{
    friend class ServerManager;
    NetClient(std::shared_ptr<Client> self):self{self}{}
    std::shared_ptr<Client> self;
    QString is_online {"offline"};
    std::vector<CommandMessage> pending_messages;
};

class ChatRoom{
public:
    std::vector<std::shared_ptr<Client>> participants;
};

class ServerManager{
public:
    ServerManager(Manager&);
    Manager& mgr;
    std::vector<NetClient> net_clients;
    ChatRoom chat_room;
};


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
//    Manager& mgr;
//    ServerManager server_mgr;
};


#endif // SERVER_H
