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
class ChatRoom;
class NetClient;

struct ChatMessage{
    const QString ip;
    const QString port;
    const QString id;
    const QString name;
    const QString message;
    const QString time;
};
class ChatRoom{
    friend class ServerManager;
    const unsigned int chat_room_no;

    std::vector<NetClient*> participants;
    std::vector<ChatMessage> logs;
    std::string name;

public:
    ChatRoom(decltype(chat_room_no) c, std::string name):chat_room_no{c}, name{name}{}
    bool operator == (const unsigned int no){
        return chat_room_no==no;
    }
    const QString getName() const{
        return name.c_str();
    }
    const decltype(chat_room_no) getId() const{
        return chat_room_no;
    }
    const decltype(participants)& getParticipants() const{
        return participants;
    }
};

class NetClient{
    friend class ServerManager;
    std::shared_ptr<Client> self;
    bool is_online=false;
    const QTcpSocket* socket=nullptr;
    std::vector<ChatRoom*> attending_rooms;
public:
    NetClient(std::shared_ptr<Client> self):self{self} {}
    const bool isOnline() const{
        return is_online;
    }
    const Client& getClient() const{
        return *self.get();
    }
    void removeRoom(unsigned int c){
        for(auto r = attending_rooms.begin(); r!=attending_rooms.end(); r++){
            if(*(*r)==c){
                attending_rooms.erase(r);
                break;
            }
        }
    }
};



class ServerManager{
private:

    Manager& mgr;
    Server* server;
    std::map<const std::string, std::shared_ptr<NetClient>> net_clients;
    std::vector<ShowChatView*> chat_views;
    QHash<const QTcpSocket*, NetClient*> socket_to_nclient;
    std::list<ChatRoom> chat_rooms;
    unsigned int chat_room_no=1;//채팅방 번호는 1부터 시작. 0은 채팅방 번호와 관련이 없는 메시지가 쓴다.
public:
    ServerManager(Manager&);
    void setServer(Server* server_){server=server_;}
    NetClient &addClient(const std::shared_ptr<Client>& c);
    void dropClient(const unsigned int chat_room_no_, const QString id);

    bool attendClient(unsigned int chat_room_no_, NetClient &);
    decltype(chat_rooms)::reference addChatRoom(std::string title){
        return chat_rooms.emplace_back(chat_room_no++,title);
    }

    decltype(net_clients)::iterator findNetClient(QString id){
        return net_clients.find(id.toStdString());
    }
    void registerChatView(ShowChatView* view){
        chat_views.emplace_back(view);
    }
    void unregisterChatView(ShowChatView* view);

    const decltype(net_clients)& getNetClients() const{
        return net_clients;
    }
    const decltype(chat_rooms)& getChatRooms() const{
        return chat_rooms;
    }
    const unsigned int getSize() const{
        return net_clients.size();
    }
    void processMessage(const QTcpSocket * const socket, QByteArray);


private:
    void login(const QTcpSocket* const socket, const QString &rmsg);
    void logOut(const QTcpSocket* const socket);
    void chatTalk(const QTcpSocket* const socket ,const QByteArray& data );
    void fileTransmission(const QTcpSocket* const socket, QByteArray&);
    void notify();


};

#endif // SERVERMANAGER_H
