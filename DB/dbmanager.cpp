//#include "dbmanager.h"
//#include <QSqlDatabase>
//#include <QFile>
//#include <QSqlQuery>
//#include <QSqlError>
//using namespace DBM;
//DBManager::DBManager(QString db_name) : db_name{db_name}
//{
//    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName(db_name);

//    QFile create_query_file{":/DB/Queries/createTable.txt"};
//    if(!create_query_file.open(QIODevice::ReadOnly|QIODevice::Text))
//        throw -1;// todo

//    QString create_query = create_query_file.readAll();
//    auto query_result = db.exec(create_query);
//    if(query_result.lastError().isValid())
//        throw -1;

//}


//unsigned int DBManager::ClientManager::getSize() const{
//    QSqlQuery query{"select count(id) from client;",db};
//    if(query.next()){
//        return query.value(0).toUInt();
//    }
//    else{
//        return 0;
//    }
//}

//Iterator<CM::Client>& DBManager::ClientManager::getClients() {
//    return itr;
//}

//DBManager::ClientManager::iterator::iterator (QSqlDatabase db) : st{0,db}
//{
//    QSqlQuery query{"select count(id) from client;",db};
//    int end=0;
//    if(query.next()){
//         end = query.value(0).toUInt();
//    }
//    ed.set(end,db);
//}



//QSqlRecord DBManager::ClientManager::iterator::Itr::getPtr() const{
//    QSqlQuery query{"select * from client;",db};
//    query.seek(ptr);
//    return query.record();
//}

//const CM::Client DBManager::ClientManager::iterator::Itr::operator*() const {
//    const QSqlRecord record = getPtr();
//    string id = record.value("id").toString().toStdString();
//    string name = record.value("name").toString().toStdString();
//    string phone_number = record.value("phone_number").toString().toStdString();
//    string address = record.value("address").toString().toStdString();
//    return CM::Client(id,name,phone_number,address);
//}
//void DBManager::ClientManager::iterator::Itr::operator++(){
//    ++ptr;
//}
//bool DBManager::ClientManager::iterator::Itr::operator!=(IteratorElem& rhs){
//    auto it = static_cast<DBManager::ClientManager::iterator::Itr&>(rhs);
//    return getPtr()!=it.getPtr();
//}
//bool DBManager::ClientManager::iterator::Itr::operator==(IteratorElem& rhs){
//    auto it = static_cast<DBManager::ClientManager::iterator::Itr&>(rhs);
//    return getPtr()==it.getPtr();
//}
