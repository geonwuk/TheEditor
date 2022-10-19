#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QList>
#include <vector>
#include "Manager/ClientManager.h"
#include <QHash>
#include "Network/message.h"
#include <QTcpSocket>

using CM::Client;
class ChatRoom;
class QTcpServer;

class QLabel;
class Manager;
class ShowChatView;
struct NetClient{
    NetClient(std::shared_ptr<Client> self):self{self}{}
    std::shared_ptr<Client> self;
    bool is_online=false;
    std::vector<Message> pending_messages;
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

private:
    Manager& mgr;
    std::vector<NetClient> net_clients;
    ChatRoom chat_room;
public:
    decltype(net_clients)::iterator findNetClient(QString id){
        return find_if(net_clients.begin(),net_clients.end(), [=](NetClient& nc){
            qDebug()<<"ID "<<nc.self->getId().c_str();
            return (id == (nc.self->getId().c_str())) ? true : false;
        });
    }
    decltype(net_clients)::iterator end(){
        return net_clients.end();
    }

    template<typename F>
    void updateView(F);
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
  //  QList<QTcpSocket*> clientList;

    struct Data {
        bool is_ready=false;
        QByteArray data;
        quint64 target_size=0;
    };

    QHash<QTcpSocket*, Data> socke_data;
    QHash<QTcpSocket*, NetClient*> ip_to_nclient;
//    Manager& mgr;
    ServerManager& mgr;
};



#endif // SERVER_H
