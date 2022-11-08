#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include "Model/model.h"

class QSqlDatabase;

namespace DBM {
class DBManager
{
public:
    DBManager(QString db_name);

    class ClientManager : public ClientModel{
    public:
        ClientManager(const QSqlDatabase& db):db{db}{}
        unsigned int getSize() const override;
        Iterator<CM::Client>& getClients() const override;
        const CM::Client& findClient(const CM::CID) const override;
        void addClient(string, string, string, string) override;
        bool modifyClient(const CM::CID, const CM::Client ) override;
        bool eraseClient(const CM::CID) override;
        std::shared_ptr<CM::Client> copyClient(const CM::CID) const override;
    private:
        const QSqlDatabase& db;

    };

private:
    QString db_name;
};
}


#endif // DBMANAGER_H
