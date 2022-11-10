#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include "Model/model.h"
#include <QSqlDatabase>

namespace DBM {
class DBManager
{
public:
    DBManager(QString db_name);
    class ClientManager : public ClientModel{
    public:
        class iterator;
        unsigned int getSize() const override;
        const CM::Client findClient(const CM::CID) const override;
        bool addClient(std::string, std::string, std::string, std::string) override;
        bool modifyClient(const CM::CID, const CM::Client ) override;
        bool eraseClient(const CM::CID) override;
        CM::Client copyClient(const CM::CID) const override;
        IteratorPTR<CM::Client> begin() override;
        IteratorPTR<CM::Client> end() override;
    private:
        class CIterator : public Iterator<CM::Client> {
            using Itr_type = int;
        public:
            friend class ClientManager;
            CIterator(Itr_type p) : ptr{ p } {}
            ~CIterator() {}
            const CM::Client operator*() const override ;
            void operator++() override;
//            CIterator& operator= (Iterator& rhs) override;
            bool operator!=(Iterator& b) override;
            bool operator==(Iterator& b) override;
            QSqlRecord getPtr() const;
        private:
            Itr_type ptr;
            static QSqlDatabase& db;
        };
    private:
        const QObject* parent;
        const QSqlDatabase& db;


    public:
        ClientManager(const QSqlDatabase& db):db{db} {
            CIterator::db=db;
        }
    private:
    };

private:
    QString db_name;
    QSqlDatabase db;
};
}


#endif // DBMANAGER_H
