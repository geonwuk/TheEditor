#include "logmanagement.h"
#include <QFile>
#include <thread>
#include <QTextStream>
using namespace std;
using ChatMessage = ServerManager::ChatMessage;
bool LogBroker::put(InputData data){
    bool re;
    tie(ignore, re) = msgs.emplace(data.session, data);
    return re;
}

LogBroker::OutputData LogBroker::get(int session){
    auto itr = msgs.find(session);
    if(itr==msgs.end()){
        return {false,ChatMessage()};
    }
    auto& lock = itr->second.lock;
    auto& dt = itr->second.data;        //dt=data
    bool lockable = lock.try_lock();    //거짓이면 이미 누가 락을 걸었다는 의미
    if(lockable){
        --dt.life_time;                 //락을 건 상태에서 감소시키므로 data_race가 일어나지 않는다
        lock.unlock();
        if(dt.life_time==0){            //소비자 수만큼 소비되었으므로 데이터를 지운다
            ChatMessage MSG = itr->second.data.msg; //지우기 전에 정보를 저장(리턴해야 하므로)
            msgs.erase(itr);
            return {true, MSG};
        }
        else{
            return {true, itr->second.data.msg};
        }
    }
    else{
        return {false,ChatMessage()};       //락이 이미 걸어졌으므로 나중에 다시 소비하도록 함
    }
}

void LogProducer::produce(ChatMessage msg, int life_time){  //life_time:데이터가 소비될 수(소비자 수)
    broker.put({session++ ,msg, life_time});
}


LogConsumer::LogConsumer(LogBroker& broker, QString file_name) : file_name{file_name},broker{broker} {
    file = new QFile(file_name);
    file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append );
}
void LogConsumer::consume(int session){
    thread t {&LogConsumer::threaded_consume,this,session};     //쓰레드로 데이터를 소비합니다
    t.detach();
}
static void little_sleep(std::chrono::microseconds us){
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}
void LogConsumer::threaded_consume(int session){
    QTextStream out(file);
    bool ready=false;
    LogBroker::OutputData output;
    while(!ready){
        output = broker.get(session);
        if(!output.is_ready){                               //만약 lock이 걸려 있다면
            little_sleep(std::chrono::microseconds(100));   //100밀리초만큼 쉬고 다시 데이터 접근을 시도한다
        }
        else{
            ready = true;
        }
    }
    auto& data = output.msg;
    out<<data.ip<<","<<data.port<<","<<data.id<<","<<data.name<<","<<data.message<<","<<data.time<<"\n";
}
LogConsumer::~LogConsumer(){
    file->close();
    delete file;
}
