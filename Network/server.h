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

#include <vector>
#include "Manager/ClientManager.h"

using CM::Client;
class ChatRoom;

class CommandMessage{
public:
    enum class type {Invitation,Drop};

};

class NetClient{
    std::shared_ptr<Client> self;
    std::vector<std::shared_ptr<ChatRoom>> attending_rooms;
    bool is_online;
    std::vector<CommandMessage> pending_messages;
};

class ChatRoom{
    std::vector<std::shared_ptr<Client>> participants;
};

class ServerManager{
    std::vector<std::shared_ptr<NetClient>> net_clients;
    std::vector<std::shared_ptr<NetClient>> online_clients;

};

#endif // SERVER_H
