#include "dbmanager.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
using namespace DBM;
DBManager::DBManager(QString db_name) : db_name{db_name}
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
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
    if(query.isValid()){
        return query.value(0).toUInt();
    }
    else{
        return 0;
    }
}
