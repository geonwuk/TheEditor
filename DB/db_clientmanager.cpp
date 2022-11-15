#include "db_clientmanager.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <string>
extern const char name2[] = "Client";
using namespace DBM;
using std::string;
//QSqlDatabase ClientManager::db {};


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

//IteratorPTR<CM::Client> ClientManager::begin(){
//    return IteratorPTR<CM::Client>{ new CIterator{0} };
//}
//IteratorPTR<CM::Client> ClientManager::end(){
//    QSqlQuery query{"select count(id) from client;",db};
//    if(query.next()){
//        return IteratorPTR<CM::Client>{ new CIterator{query.value(0).toInt()} };
//    }
//    else{
//        return IteratorPTR<CM::Client>{ new CIterator{0} };
//    }
//}

//QSqlRecord ClientManager::CIterator::getPtr() const{
//    QSqlQuery query{"select * from client order by id;",ClientManager::getDb()};
//    query.seek(ptr);
//    return query.record();
//}

void ClientManager::loadClients(QString file_name) {
    try{
        DBM::ClientManager db_data{"load_client",file_name};
        for(const auto& c : db_data){
            bool re = addClient(c.getId(),c.getName(),c.getPhoneNumber(),c.getAddress());
            if(!re)
                throw -1;
        }
    }
    catch(...){
        throw;
    }
}

const CM::Client ClientManager::CIterator::operator*() const {
    const QSqlRecord record = getPtr();
    string id = record.value("id").toString().toStdString();
    string name = record.value("name").toString().toStdString();
    string phone_number = record.value("phone_number").toString().toStdString();
    string address = record.value("address").toString().toStdString();
    return CM::Client(id,name,phone_number,address);
}
//void ClientManager::CIterator::operator++(){
//    ++ptr;
//}
//bool ClientManager::CIterator::operator!=(Iterator& rhs){
//    auto it = static_cast<ClientManager::CIterator&>(rhs);
//    return getPtr()!=it.getPtr();
//}
//bool ClientManager::CIterator::operator==(Iterator& rhs){
//    auto it = static_cast<ClientManager::CIterator&>(rhs);
//    return getPtr()==it.getPtr();
//}

IteratorPTR<CM::Client> ClientManager::begin() const{
    return IteratorPTR<CM::Client>{ new CIterator{0} };
}
IteratorPTR<CM::Client> ClientManager::end() const{
  QSqlQuery query{QString("select count(id) from ") + name2, db};
  if (query.next()) {
    return IteratorPTR<CM::Client>{new CIterator{query.value(0).toInt()}};
  } else {
    return IteratorPTR<CM::Client>{new CIterator{0}};
  }
}
