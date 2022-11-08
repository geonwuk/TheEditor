#include "dbmanager.h"
#include <QSqlDatabase>
#include <QFile>
DBManager::DBManager(QString db_name) : db_name{db_name}
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_name);


}
