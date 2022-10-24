#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H
#include "Manager/ClientManager.h"
#include <memory>
#include <QHash>
#include "Network/message.h"
class QTcpSocket;
using CM::Client;


class ShowChatView;
class Server;
class Manager;
class ServerManager{
public:
    struct ChatMessage{
        const QString ip;
        const QString port;
        const QString id;
        const QString name;
        const QString message;
        const QString time;
    };
private:
    class NetClient{
        friend class ServerManager;
        std::shared_ptr<Client> self;
        bool is_online=false;
        std::vector<Message> pending_messages;
        const QTcpSocket* socket=nullptr;
        const unsigned int log_no;
    public:
        NetClient(std::shared_ptr<Client> self, const unsigned int log_no ):self{self}, log_no{log_no} {}
        const bool isOnline() const{
            return is_online;
        }
        const Client& getClient() const{
            return *self.get();
        }
    };
    Manager& mgr;
    Server* server;
    std::vector<ChatMessage> logs;
    unsigned int log_no=1;
    std::map<const std::string, std::shared_ptr<NetClient>> net_clients;
    std::vector<ShowChatView*> chat_views;
    QHash<const QTcpSocket*, NetClient*> socket_to_nclient;
public:
    ServerManager(Manager&);
    void setServer(Server* server_){server=server_;}

    void addClient(const std::shared_ptr<Client>& c);
    void dropClient(QString);



    decltype(net_clients)::iterator findNetClient(QString id){
        return net_clients.find(id.toStdString());
    }




    void registerChatView(ShowChatView* view){
        chat_views.emplace_back(view);
    }
    void unregisterChatView(ShowChatView* view);

    decltype(net_clients)::const_iterator begin() const{
        return net_clients.cbegin();
    }
    decltype(net_clients)::const_iterator end() const{
        return net_clients.cend();
    }
    const unsigned int getSize() const{
        return net_clients.size();
    }

    void processMessage(const QTcpSocket * const socket, QByteArray);


private:
    void login(const QTcpSocket* const socket, const QString &rmsg);
    void logOut(const QTcpSocket* const socket);
    void chatTalk(const QTcpSocket* const socket , const QString& );
    void fileTransmission(const QTcpSocket* const socket, QByteArray&);
    void notify();
};

#endif // SERVERMANAGER_H
