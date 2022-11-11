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
        //ClientManager(QString db_name);
        static const QSqlDatabase& getDb() {return db;}
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
        class CIterator : public DBIterator<CM::Client> {
            using Itr_type = int;
        public:
            using DBIterator::DBIterator;
            //friend class ClientManager;
            //CIterator(Itr_type p) : ptr{ p } {}
            ~CIterator() {}
            const CM::Client operator*() const override ;
//            void operator++() override;
//            CIterator& operator= (Iterator& rhs) override;
//            bool operator!=(Iterator& b) override;
//            bool operator==(Iterator& b) override;
//            QSqlRecord getPtr() const;
        private:
            Itr_type ptr;
        };
public:



    private:
        const QObject* parent;
        QString db_name;
        //static QSqlDatabase db;
    };


}


#endif // DB_CLIENTMANAGER_H
