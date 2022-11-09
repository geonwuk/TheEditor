#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include "Model/model.h"
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>
class QSqlDatabase;

//namespace DBM {
//class DBManager
//{
//public:
//    DBManager(QString db_name);


//    class ClientManager : public ClientModel{
//    public:
//        class iterator;
//        ClientManager(const QSqlDatabase& db):db{db},itr{db}{}
//        unsigned int getSize() const override;
//        const CM::Client& findClient(const CM::CID) const override;
//        bool addClient(string, string, string, string) override;
//        bool modifyClient(const CM::CID, const CM::Client ) override;
//        bool eraseClient(const CM::CID) override;
//        //std::shared_ptr<CM::Client> copyClient(const CM::CID) const override;

//        class iterator : public Iterator<CM::Client>{
//            using Itr_type = int;
//            struct Itr : public IteratorElem<CM::Client> {
//                Itr(){}
//                Itr(Itr_type p , QSqlDatabase db) : ptr{p}, db{db} {}
//                const CM::Client operator*() const override;
//                void operator++() override ;
//                bool operator!=(IteratorElem& ) override;
//                bool operator==(IteratorElem& ) override;
//                std::unique_ptr<CM::Client> operator->() override {
//                    return std::unique_ptr<CM::Client>();
//                }
//                void set(Itr_type p, QSqlDatabase db){ptr=p;this->db=db;}
//            private:
//                QSqlRecord getPtr() const;
//                Itr_type ptr;
//                QSqlDatabase db;
//            };
//            Itr st;
//            Itr ed;
//        public:
//            iterator(QSqlDatabase db);
//            IteratorElem<CM::Client>& begin() override { return st;}
//            IteratorElem<CM::Client>& end() override { return ed;}
//        };

//    private:
//        const QObject* parent;
//        const QSqlDatabase& db;
//        iterator itr;

//    };

//    ClientManager getClientManager() const{return ClientManager{db};}
//private:
//    QString db_name;
//    QSqlDatabase db;
//};
//}


#endif // DBMANAGER_H
