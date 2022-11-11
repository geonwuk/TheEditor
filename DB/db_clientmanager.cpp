#include "db_clientmanager.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <string>
using namespace DBM;
using std::string;
//QSqlDatabase ClientManager::db {};
extern const char name2[] = "Client";
//ClientManager::ClientManager(QString db_name) : db_name{db_name}
//{
//    ClientManager::db = QSqlDatabase::addDatabase("QSQLITE","CLIENT");
//    db.setDatabaseName(db_name);
//    if(!db.open()){
//        qDebug()<<"not open";
//    }

//    QFile create_query_file{":/DB/Queries/createClientTable.txt"};
//    if(!create_query_file.open(QIODevice::ReadOnly|QIODevice::Text))
//        throw -1;// todo

//    QString create_query = create_query_file.readAll();

//    create_query.remove('\n');

//    qDebug()<<create_query;
//    auto query_result = db.exec(create_query);
//    if(query_result.lastError().isValid())
//        throw -1;
//}


unsigned int ClientManager::getSize() const{
    QSqlQuery query{"select count(id) from client;",db};
    if(query.next()){
        return query.value(0).toUInt();
    }
    else{
        return 0;
    }
}

const CM::Client ClientManager::findClient(const CM::CID id) const {
    QSqlQuery query{db};
    query.prepare("select * from client where id = :id;");
    query.bindValue(":id",QString(id.c_str()));
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
    QSqlQuery query{db};
    query.prepare("insert into client values (:id, :name, :phone_number, :address);");
    query.bindValue(":id", id.c_str());
    query.bindValue(":name", name.c_str());
    query.bindValue(":phone_number", phone_number.c_str());
    query.bindValue(":address", address.c_str());
    return query.exec();
}

bool ClientManager::modifyClient(const CM::CID id, const CM::Client client){
    QSqlQuery query{db};
    query.prepare("update client set name=:name, phone_number=:phone_number, address=:address where id=:id;");
    query.bindValue(":id",id.c_str());
    query.bindValue(":name",client.getName().c_str());
    query.bindValue(":phone_number",client.getPhoneNumber().c_str());
    query.bindValue(":address",client.getAddress().c_str());
    return query.exec();
}

bool ClientManager::eraseClient(const CM::CID id){
    QSqlQuery query{db};
    query.prepare("delete from client where id=:id;");
    query.bindValue(":id",id.c_str());
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

IteratorPTR<CM::Client> ClientManager::begin(){
    return IteratorPTR<CM::Client>{ new CIterator{0} };
}
IteratorPTR<CM::Client> ClientManager::end() {
  QSqlQuery query{QString("select count(id) from ") + name2, db};
  if (query.next()) {
    return IteratorPTR<CM::Client>{new CIterator{query.value(0).toInt()}};
  } else {
    return IteratorPTR<CM::Client>{new CIterator{0}};
  }
}
