#ifndef LOGPRODUCER_H
#define LOGPRODUCER_H
#include "Network/servermanager.h"
#include <mutex>
class QFile;
class LogConsumer;
class LogBroker{            //생산자와 소비자 사이를 조율하는 브로커
public:
    struct InputData{
        int session;        //데이터 ID
        ServerManager::ChatMessage msg;    //채팅 메시지
        int life_time;      //소비자의 수(데이터가 소비될 수) 데이터 소비가 life_time만큼 이루어져야 데이터를 지울 수 잇다
    };
    struct OutputData{
        bool is_ready;      //is_ready가 false이면 mutex가 lock 되어있으므로 소비자는 sleep을 한다
        ServerManager::ChatMessage msg;    //채팅 메시지
    };
    bool put(InputData);    //생성자가 데이터를 생성한 후 브로커에게 전달하는 함수
    OutputData get(int);    //생성된 데이터를 소비하는 함수
    bool isEmpty() const {return msgs.empty();}     //데이터가 소비되는 중에 소비자가 지워질 수 있으므로 소비자를 지우기 전에 empty를 먼저 확인하고 지운다
private:
    struct Data{
        InputData data;
        std::mutex lock;    //life_time을 감소시킬 떄 이 lock을 먼저 해야한다. 만약 하지 않으면 data race가 발생할 수 있다
        Data(){}
        Data(InputData d) : data{d} {}
    };
    std::map<int, Data> msgs;
};

class LogProducer       //생산자
{
public:
    LogProducer(LogBroker& broker) : broker{broker} {}
    void produce(ServerManager::ChatMessage, int life_time);
    int session = 0;                                        //데이터 ID로 데이터 생성시마다 ++이 된다. 소비자는 이 session으로 데이터를 구분한다.
private:
    LogBroker& broker;
};

class LogConsumer{
public:
    LogConsumer(LogBroker& broker, QString file_name);
    void consume(int session);
    ~LogConsumer();
    QString file_name;
private:
    void threaded_consume(int session);
    LogBroker& broker;
    QFile* file;
};

#endif // LOGPRODUCER_H
