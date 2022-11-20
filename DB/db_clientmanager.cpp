#include "db_clientmanager.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <string>
extern const char CLIENT_TABLE_NAME[] = "Client";
using namespace DBM;
using std::string;
//QSqlDatabase ClientManager::db {};

ClientManager::ClientManager(QString connection_name, QString file_name):DBManager{connection_name,file_name} {
    QString create_query = "Create TABLE IF NOT EXISTS Client("
                           "id varchar(20) PRIMARY KEY,"
                           "name varchar(20),"
                           "phone_number varchar(20),"
                           "address varchar(50));";
    auto query_result = db.exec(create_query);
    if(query_result.lastError().isValid())
        throw DBM::ERROR_WHILE_LOADING{"Client"};
    QSqlQuery columnNamesQuery {QString("SELECT name FROM PRAGMA_TABLE_INFO('")+CLIENT_TABLE_NAME+"');",db};
    while(columnNamesQuery.next()){
        column_names<<columnNamesQuery.value(0).toString();
    }
    column_names.removeFirst();//ID 칼럼 삭제
}
unsigned int ClientManager::getSize() const{
    auto query = DBManager::getSize();
    query.exec();
    return query.next() ? query.value(0).toUInt() : 0;
}
const CM::Client ClientManager::findClient(const CM::CID id) const {
    auto query = find(id.c_str());
    query.exec();
    if(query.next()){
        string id = query.value("id").toString().toStdString();
        string name = query.value("name").toString().toStdString();
        string phone_number = query.value("phone_number").toString().toStdString();
        string address = query.value("address").toString().toStdString();
        return CM::Client(id,name,phone_number,address);
    }
    else{
        return CM::no_client;
    }
}
bool ClientManager::addClient(std::string id, std::string name, std::string phone_number, std::string address){
    auto query = add(id.c_str(),name.c_str(),phone_number.c_str(),address.c_str());
    qDebug()<<query.lastQuery();
    return query.exec() && !query.lastError().isValid();
}
void ClientManager::loadClient(const std::vector<CM::Client>& clients_to_add){
    unsigned int line=0;
    for(const auto& c : clients_to_add ){
        if(!addClient(c.getId(),c.getName(),c.getPhoneNumber(),c.getAddress())){
            throw ClientModel::ERROR_WHILE_LOADING{line};
        }
        line++;
    }
}
void ClientManager::checkSafeToLoad(const std::vector<CM::Client>& clients_to_add){
    unsigned int line=0;
    for(const auto& c : clients_to_add ){
        if(findClient(c.getId())==CM::no_client){
            throw ClientModel::ERROR_WHILE_LOADING{line};
        }
        line++;
    }
}
bool ClientManager::modifyClient(const CM::CID id, const CM::Client client){
    auto query = modify(id.c_str(), client.getName().c_str(), client.getPhoneNumber().c_str(), client.getAddress().c_str());
    qDebug()<<query.lastQuery();
    return query.exec();
}

bool ClientManager::eraseClient(const CM::CID id){
    auto query = erase(id.c_str());
    return query.exec();
}
CM::Client ClientManager::copyClient(const CM::CID cid) const{
    QSqlQuery query{db};
    query.prepare("select * from client where id = :id;");
    query.bindValue(":id",cid.c_str());
    query.exec();
    query.seek(0);
    const QSqlRecord record = query.record();
    string id = record.value("id").toString().toStdString();
    string name = record.value("name").toString().toStdString();
    string phone_number = record.value("phone_number").toString().toStdString();
    string address = record.value("address").toString().toStdString();
    return CM::Client(id,name,phone_number,address);
}
const CM::Client ClientManager::CIterator::operator*() const {
    const QSqlRecord record = getPtr();
    string id = record.value("id").toString().toStdString();
    string name = record.value("name").toString().toStdString();
    string phone_number = record.value("phone_number").toString().toStdString();
    string address = record.value("address").toString().toStdString();
    return CM::Client(id,name,phone_number,address);
}
IteratorPTR<CM::Client> ClientManager::begin() const{
    return IteratorPTR<CM::Client>{ new CIterator{0,db} };     //쿼리문의 첫번째 index는 0부터 시작하므로 0으로 초기화합니다
}
IteratorPTR<CM::Client> ClientManager::end() const{
  QSqlQuery query{QString("select count(id) from ") + CLIENT_TABLE_NAME, db};               //id 레코드 개수를 알기 위한 쿼리를 실행합니다
  if (query.next()) {
    return IteratorPTR<CM::Client>{new CIterator{query.value(0).toInt(),db}};      //레코드 개수 만큼 end로 설정합니다
  } else {
    return IteratorPTR<CM::Client>{new CIterator{0,db}};                           //만약 쿼리 결과 레코드가 하나도 없다면 0입니다
  }
}
