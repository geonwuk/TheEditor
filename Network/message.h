#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
enum REQUEST{
    Chat_Login,
    Chat_In,
    Chat_Talk,
    Chat_Close,
    Chat_LogOut,
    Chat_Invite,
    Chat_KickOut,
    Chat_FileTransform
};
enum RESPOND{
    NO_ID,
    SUCCESS,
    BAD_REQUEST
};

class Message {
public:
    Message(QString, REQUEST);
    Message(QString, RESPOND);

    operator const QByteArray() const;
private:
    char type;
    QByteArray data;
};




#endif // MESSAGE_H
