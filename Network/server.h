#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QList>
#include <vector>
#include "Manager/ClientManager.h"
#include <QHash>
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
    NetClient(){}
    NetClient(std::shared_ptr<Client> self):self{self}{}
    std::shared_ptr<Client> self;
    QString is_online {"offline"};
    std::vector<CommandMessage> pending_messages;
    QTcpSocket* socket;
};

class ChatRoom{
public:
    std::vector<std::shared_ptr<Client>> participants;
};

class ServerManager{
public:
    ServerManager(Manager&);
    void addClient(const std::shared_ptr<Client>& c){
        net_clients.emplace_back(c);
    }

    const std::vector<NetClient>& getNetClients(){
        return net_clients;
    }

public:
    Manager& mgr;
    std::vector<NetClient> net_clients;
    ChatRoom chat_room;
};

typedef struct {
    char type;
    char data[1023];
} chatProtocolType;

typedef enum {
    Chat_Login,
    Chat_In,
    Chat_Talk,
    Chat_Close,
    Chat_LogOut,
    Chat_Invite,
    Chat_KickOut,
    Chat_FileTransform
} Chat_Status;

enum RESPOND{
    NO_ID,
    SUCCESS

};


class Server : public QWidget
{
    Q_OBJECT

public:
    Server(ServerManager&, QWidget *parent = nullptr);
    ~Server();
private slots:
    void clientConnect();
    void readData();
private:
    QTcpServer* tcpServer;
    QList<QTcpSocket*> clientList;
    QHash<QString, NetClient*> ipToClient ;
//    Manager& mgr;
    ServerManager& mgr;
};


#endif // SERVER_H
