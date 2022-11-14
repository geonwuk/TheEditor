#ifndef DB_CLIENTMANAGER_H
#define DB_CLIENTMANAGER_H

#include <QString>
#include "Model/model.h"
#include <QSqlDatabase>
#include "DB/dbmanager.h"
extern const char name2[];
namespace DBM {
class ClientManager : public ClientModel, public DBManager<name2>{
public:
    ClientManager():DBManager{} {}
    ClientManager(QString file_name):DBManager{file_name} {}
    static const QSqlDatabase& getDb() {return db;}
    unsigned int getSize() const override;
    const CM::Client findClient(const CM::CID) const override;
    bool addClient(std::string, std::string, std::string, std::string) override;
    bool modifyClient(const CM::CID, const CM::Client ) override;
    bool eraseClient(const CM::CID) override;
    CM::Client copyClient(const CM::CID) const override;
    IteratorPTR<CM::Client> begin() override;
    IteratorPTR<CM::Client> end() override;

private:
    class CIterator : public DBIterator<CM::Client> {
    public:
        using DBIterator::DBIterator;
        const CM::Client operator*() const override ;
    };
    //static QSqlDatabase db;
};

}
#endif // DB_CLIENTMANAGER_H
