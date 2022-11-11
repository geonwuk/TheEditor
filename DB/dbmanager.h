#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include "Model/model.h"
#include <QSqlRecord>
template<const char* table_name>
class DBManager
{
public:

    DBManager(){
        DBManager::db = QSqlDatabase::addDatabase(DBType,table_name);
        db.setDatabaseName(db_name);
        if(!db.open()){
            qDebug()<<"not open";
        }
        QString file_name{":/DB/Queries/create"};
        file_name+=(QString(table_name)+QString("Table.txt"));
        qDebug()<<"filename"<<file_name;
        QFile create_query_file{file_name};
        if(!create_query_file.open(QIODevice::ReadOnly|QIODevice::Text))
            throw -1;// todo

        QString create_query = create_query_file.readAll();

        create_query.remove('\n');

        qDebug()<<create_query;
        auto query_result = db.exec(create_query);
        if(query_result.lastError().isValid())
            throw -1;


//        QSqlQuery countColumn {QString("SELECT count(*) FROM PRAGMA_TABLE_INFO('")+table_name+"')",db};
//        int num_of_columns = countColumn.next() ? countColumn.value(0).toInt() : 0;

        QSqlQuery columnNamesQuery {QString("SELECT name FROM PRAGMA_TABLE_INFO('")+table_name+"');",db};
        while(columnNamesQuery.next()){
            qDebug()<<columnNamesQuery.value(0).toString();
            column_names<<columnNamesQuery.value(0).toString();
        }

    }
    QSqlQuery getSize() const{
        return QSqlQuery{QString("select count(id) from ")+QString(table_name),db};
    }
    QSqlQuery find(const QString id) const {
        QSqlQuery query{db};
        query.prepare(QString("select * from ")+QString(table_name)+QString(" where id = :id;"));
        query.bindValue(":id",id);
        return query;
    }
    template <typename T>
    QSqlQuery bindValue(QSqlQuery query, T arg) {
        query.addBindValue(arg);
        return query;
    }
    template <typename T,typename... Types>
    QSqlQuery bindValue(QSqlQuery query, T arg, Types... args) {
        query.addBindValue(arg);
        return bindValue(query, args...);
    }
    template <std::size_t sz>
    QString appendQueryString(QString query){
        query+="?,";
        return appendQueryString<sz-1>(query);
    }
    template <>
    QString appendQueryString<0>(QString query){
        query.chop(1);
        query+=");";
        return query;
    }
    template <typename... Args>
    QSqlQuery add(Args... args) {
        QString query_string ((QString("insert into ")+table_name+" values ("));
        query_string = appendQueryString<sizeof...(args)>(query_string);
        qDebug()<<query_string;
        QSqlQuery query{db};
        query.prepare(query_string);
        return bindValue(query, args...);
    }


    template <typename... Fields>
    QSqlQuery modify(QString id,Fields... args){
        QString query_string ((QString("update ")+table_name+" set "));
        for(const auto& name : column_names){
            query_string+=name;
            query_string+="=?,";
        }
        query_string.chop(1);
        query_string+=" where id=?;";
        QSqlQuery query{db};
        query.prepare(query_string);
        query = bindValue(query, args...);
        query.addBindValue(id);
        qDebug()<<"modify query: "<<query.lastQuery();
        return query;
    }

    QSqlQuery erase(QString id){
        QSqlQuery query{db};
        query.prepare(QString("delete from ")+table_name+" where id=:id;");
        query.bindValue(":id",id);
        return query;
    }

    template<typename T>
    struct DBIterator : public Iterator<T>{
        using Itr_type = int;
        DBIterator(Itr_type p) : ptr{ p } {}
        void operator++(){
            ++ptr;
        }
        bool operator!=(Iterator<T>& rhs){
            auto it = static_cast<DBIterator&>(rhs);
            return getPtr()!=it.getPtr();
        }
        bool operator==(Iterator<T>& rhs){
            auto it = static_cast<DBIterator&>(rhs);
            return getPtr()==it.getPtr();
        }
        const T operator*() const override{
            return T{};
            //static_assert(false,"d");
        }
        QSqlRecord getPtr() const{
            QSqlQuery query{QString("select * from ")+table_name+" order by id;",db};
            query.seek(ptr);
            return query.record();
        }

    private:
        Itr_type ptr;
    };




protected:

    QString db_name{"test"};
    QString DBType{"QSQLITE"};
    static QSqlDatabase db;
    QStringList column_names;
};

template <const char* table_name>
QSqlDatabase DBManager<table_name>::db {};

#endif // DBMANAGER_H
