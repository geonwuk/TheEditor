#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QList>
#include <vector>
#include <map>
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
    QTcpSocket* socket=nullptr;
};


class ServerManager{
public:
    ServerManager(Manager&);
    void addClient(const std::shared_ptr<Client>& c){
        qDebug()<<"ADDING NEW NC";
        auto client = std::make_shared<NetClient>(c);
        net_clients.emplace(c->getId(), client);
    }
    void addLog(QTcpSocket* socket, QString data);

    struct ChatMessage{
        QString ip;
        QString port;
        QString id;
        QString name;
        QString message;
        QString time;
    };

private:
    Manager& mgr;
    std::vector<ChatMessage> logs;
    std::map<std::string, std::shared_ptr<NetClient>> net_clients;
    std::vector<ShowChatView*> chat_views;
    QHash<QTcpSocket*, NetClient*> ip_to_nclient;


public:
    decltype(net_clients)::iterator findNetClient(QString id){
        return net_clients.find(id.toStdString());
    }
    decltype(net_clients)::iterator noClient(){
        return net_clients.end();
    }
    void login(QTcpSocket* socket, decltype(net_clients)::iterator itr);
    NetClient* socketToNetClient(QTcpSocket* socket){
        auto it = ip_to_nclient.find(socket);
        return it==ip_to_nclient.end() ? nullptr : *it;
    }
    void registerChatView(ShowChatView* view){
        chat_views.emplace_back(view);
    }
    void unregisterChatView(ShowChatView* view);
    decltype(net_clients)::iterator beginClients(){
        return net_clients.begin();
    }
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

//    Manager& mgr;
    ServerManager& mgr;
};



#endif // SERVER_H
