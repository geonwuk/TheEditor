#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <memory>
#include <unordered_map>
#include <map>

#include "Model/Type.h"
#include "Network/message.h"

class QTcpSocket;
/*
Client의 정보가 수정될 경우 NetClient정보가 수정되지 않음.
NetClientModel을 새로 만들어야 함.
*/

class ShowChatView;
class Server;
class MainManager;
class ServerManager{
public:
    struct ChatMessage{
        std::string ip;
        std::string port;
        std::string id;
        std::string name;
        std::string message;
        std::string time;
    };
private:
    class NetClient{                                //기존 고객 Client에서 채팅방에 참여하는 CLient를 NetClient로 지칭
        friend class ServerManager;
        CM::Client self;
        bool is_online=false;
        std::vector<Message> pending_messages;
        const QTcpSocket* socket=nullptr;
        const unsigned int log_no;
    public:
        NetClient(CM::Client self, const unsigned int log_no ): self{self}, log_no{log_no} {}
        const bool isOnline() const{ return is_online; }
        const CM::Client& getClient() const{ return self; }
    };
    MainManager& mgr;
    Server* server;
    std::vector<ChatMessage> logs;
    unsigned int log_no;
    std::map<const std::string, NetClient> net_clients;
    std::vector<ShowChatView*> chat_views;
    std::unordered_map<const QTcpSocket*, NetClient*> socket_to_nclient;
public:
    ServerManager(MainManager&);
    ~ServerManager();
    void setServer(Server* server_){server=server_;}
    void addClient(const CM::Client& c);
    void dropClient(std::string);

    decltype(net_clients)::const_iterator findNetClient(std::string id) const{
        return net_clients.find(id);
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
    const size_t getSize() const{
        return net_clients.size();
    }

    void processMessage(const QTcpSocket * const socket, QByteArray);


private:
    void login(const QTcpSocket* const socket, const std::string &rmsg);
    void logOut(const QTcpSocket* const socket);
    void chatTalk(const QTcpSocket* const socket , const QByteArray& data );
    void fileTransmission(const QTcpSocket* const socket, QByteArray&);
    void notify();
};

#endif // SERVERMANAGER_H
