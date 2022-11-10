#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H
#include "Manager/ClientManager.h"
#include <memory>
#include <QHash>
#include "Network/message.h"
#include "Network/logthread.h"
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
    class NetClient{                                //기존 고객 Client에서 채팅방에 참여하는 CLient를 NetClient로 지칭
        friend class ServerManager;
        Client self;
        bool is_online=false;
        std::vector<Message> pending_messages;
        const QTcpSocket* socket=nullptr;
        const unsigned int log_no;
    public:
        NetClient(Client self, const unsigned int log_no ):self{self}, log_no{log_no} {}
        const bool isOnline() const{
            return is_online;
        }
        const Client& getClient() const{
            return self;
        }
    };
    Manager& mgr;
    Server* server;
    std::vector<ChatMessage> logs;
    unsigned int log_no=1;
    std::map<const std::string, NetClient> net_clients;
    std::vector<ShowChatView*> chat_views;
    QHash<const QTcpSocket*, NetClient*> socket_to_nclient;
public:
    ServerManager(Manager&);
    void setServer(Server* server_){server=server_;}

    void addClient(const Client& c);
    void dropClient(QString);



    decltype(net_clients)::iterator findNetClient(QString id){
        return net_clients.find(id.toStdString());
    }
    void registerChatView(ShowChatView* view){      //ServerManager에서 현재 생성된 chatView만 업데이트를 하기 위해 따로 등록을 받는다 Todo: 수정 필요)
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
    void chatTalk(const QTcpSocket* const socket , const QByteArray& data );
    void fileTransmission(const QTcpSocket* const socket, QByteArray&);
    void notify();
};

#endif // SERVERMANAGER_H
