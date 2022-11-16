#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>

enum REQUEST{
    Chat_Login,                 //로그인 시, 클라이언트가 서버에게 보내는 값
    Chat_Talk,                  //클라이언트가 대화를 보낼 때 서버에게 보내는 값
    Chat_LogOut,                //클라이언트가 로그아웃할 때 서버에게 보내는 값
    Chat_KickOut,               //서버가 클라이언트를 추방할 때 클라이언트에게 보내는 값
    Chat_FileTransmission,      //클라이언트가 서버에게 파일을 보낼 때 서버에게 보내는 값
    Chat_FileMsg
};

class Message {             //메시지를 보낼 때 쓰는 클래스
public:
    Message(QString, REQUEST);
    operator const QByteArray() const;      //변환 연산자를 정의해서 쉽게 쓸 수 있도록 함 (ex: socket->write(MESSAGE);)
private:
    char type;              //Request의 타입을 저장
    QByteArray data;        //메시지 데이터를 저장
};

class ReadMessage{                          //위의 Message클래스와 달리 메시지를 수신할 때 쓰는 클래스
public:
    ReadMessage(QByteArray&);
    operator const char() const;            //변환 연산자를 정의해서 switch에서 타입에 따라 분기할 수 있도록 정의함 (ex: swich(RadMessage 값) { case Chat_login: ... } )
    const QString toQString() const;        //받은 메시지의 크기와 타입을 뺀 내용을 추출할 때 쓰는 함수
    const char getType() const{             //type 게터
        return type;
    }
private:
    QByteArray& data;       //메시지 데이터를 저장
    char type;              //이 메시지의 타입이 무엇인지 저장
};

class QProgressDialog;
class QFile;
class FileMessage{      //파일 전송할 시 보내는 메시지
public:
    FileMessage(QFile*, QProgressDialog* progress_dialog, QString sender_name);
    operator const QByteArray() const;
    void updateProgress(qint64 bytes_written);
    QProgressDialog* getProgressDialog(){
        return progress_dialog;
    }
    const qint64 getFileSize() const{
        return file_size;
    }
    void completeProgress();

private:
    QString sender_name;
    QByteArray data;
    qint64 file_size;
    QFile* file;
    QProgressDialog* progress_dialog;
};



#endif // MESSAGE_H
