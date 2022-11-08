#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>

class DBManager
{
public:
    DBManager(QString db_name);

private:
    QString db_name;
};

#endif // DBMANAGER_H
