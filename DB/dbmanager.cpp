#include "dbmanager.h"
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

DBManager::DBManager(QString db_name) : db_name{db_name}
{
    db = QSqlDatabase::addDatabase("QSQLITE","CLIENT");
    db.setDatabaseName(db_name);

    QFile create_query_file{":/DB/Queries/createTable.txt"};
    if(!create_query_file.open(QIODevice::ReadOnly|QIODevice::Text))
        throw -1;// todo

    QString create_query = create_query_file.readAll();
    auto query_result = db.exec(create_query);
    if(query_result.lastError().isValid())
        throw -1;

}


unsigned int DBManager::ClientManager::getSize() const{
    QSqlQuery query{"select count(id) from client;",db};
    if(query.next()){
        return query.value(0).toUInt();
    }
    else{
        return 0;
    }
}

const CM::Client DBManager::ClientManager::findClient(const CM::CID id) const {
    QSqlQuery query{db};
    query.prepare("select * from client where id = :id;");
    query.bindValue(":id",QString(id.c_str()));
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

QSqlRecord DBManager::ClientManager::CIterator::getPtr() const{
    QSqlQuery query{"select * from client order by id;",db};
    query.seek(ptr);
    return query.record();
}
const CM::Client DBManager::ClientManager::CIterator::operator*() const {
    const QSqlRecord record = getPtr();
    string id = record.value("id").toString().toStdString();
    string name = record.value("name").toString().toStdString();
    string phone_number = record.value("phone_number").toString().toStdString();
    string address = record.value("address").toString().toStdString();
    return CM::Client(id,name,phone_number,address);
}
void DBManager::ClientManager::CIterator::operator++(){
    ++ptr;
}
bool DBManager::ClientManager::CIterator::operator!=(Iterator& rhs){
    auto it = static_cast<DBManager::ClientManager::CIterator&>(rhs);
    return getPtr()!=it.getPtr();
}
bool DBManager::ClientManager::CIterator::operator==(Iterator& rhs){
    auto it = static_cast<DBManager::ClientManager::CIterator&>(rhs);
    return getPtr()==it.getPtr();
}
