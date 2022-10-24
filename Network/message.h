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
    Chat_FileTransmission
};

class Message {
public:
    Message(QString, REQUEST);
    operator const QByteArray() const;

private:
    char type;
    QByteArray data;
};

class ReadMessage{
public:
    ReadMessage(QByteArray&);
    operator const char() const;
    const QString toQString() const;
    const char getType() const{
        return type;
    }
private:
    QByteArray& data;
    char type;

};

class QProgressDialog;
class QFile;
class FileMessage{
public:
    FileMessage(QFile*, QProgressDialog* progress_dialog);
    operator const QByteArray() const;
    void updateProgress(qint64 bytes_written);

private:
    QByteArray data;
    qint64 file_size;
    QFile* file;
    QProgressDialog* progress_dialog;
};



#endif // MESSAGE_H
