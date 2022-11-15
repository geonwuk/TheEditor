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
#include <initializer_list>
#include <sstream>
enum AddMode{
    EXPLICIT_FIELD_NAME
};
template<const char* table_name>
class DBManager
{
public:
    DBManager(QString connection_name, QString file_name="test.txt") : file_name{file_name}, connection_name{connection_name}{
        DBManager::db = QSqlDatabase::addDatabase(DBType,connection_name);
        db.setDatabaseName(file_name);
        if(!db.open()){
            qDebug()<<"not open";
        }
        QString file_name2{":/DB/Queries/create"};
        file_name2+=(QString(table_name)+QString("Table.txt"));
        qDebug()<<"filename"<<file_name2;
        QFile create_query_file{file_name2};
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
        column_names.removeFirst();//ID 칼럼 삭제

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
    QSqlQuery find(const QString table_name_, const QString id) const {
        QSqlQuery query{db};
        query.prepare(QString("select * from ")+table_name_+QString(" where id = :id;"));
        query.bindValue(":id",id);
        return query;
    }

    template <typename... Args>
    QSqlQuery add(Args... args) {
        QString query_string ((QString("insert into ")+table_name+" values ("));
        query_string = appendQueryString<sizeof...(args)>(query_string);
        query_string += ";";
        QSqlQuery query{db};
        query.prepare(query_string);
        return bindValue(query, args...);
    }
    template <typename... Args>
    QSqlQuery add(QString table_name_p, Args... args) {
        QString query_string ((QString("insert into ")+table_name_p+" values ("));
        query_string = appendQueryString<sizeof...(args)>(query_string);
        query_string += ";";
        QSqlQuery query{db};
        query.prepare(query_string);
        return bindValue(query, args...);
    }

    struct ExplicitAdd {
        ExplicitAdd(QString tb_name) : tb_name{tb_name} {}
        QSqlQuery add(std::initializer_list<std::initializer_list<QString>> args) {
            QString query_string ((QString("insert into ")+tb_name+" ("));
            for(const auto& field : args){
                query_string += *field.begin();
                query_string += ",";
            }
            query_string.chop(1);
            query_string+=") values (";
            for(int i=0; i<args.size(); i++){
                query_string+="?,";
            }
            query_string.chop(1);
            query_string+=");";
            QSqlQuery query{db};
            query.prepare(query_string);
            for(const auto& pair : args){
                auto s = pair.begin();
                query.addBindValue(*(++s));
            }
            return query;
        }
    private:
        QString tb_name;
    };

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
        qDebug()<<"err"<<query.lastError().text();
        return query;
    }

    QSqlQuery erase(QString id){
        QSqlQuery query{db};
        query.prepare(QString("delete from ")+table_name+" where id=:id;");
        query.bindValue(":id",id);
        return query;
    }

    static std::tm getDate(QString dt){
        std::string date = dt.toStdString();
        tm time;
        std::istringstream ss{ date };
        ss >> std::get_time(&time, "%D %T");
        return time;
    }

    static QString getDate(std::tm dt){
        std::ostringstream ss;
        ss<<std::put_time(&dt, "%D %T");
        return ss.str().c_str();
    }

    template<typename T>
    struct DBIterator : public Iterator<T>{
        using Itr_type = int;
        DBIterator(Itr_type p) : ptr{ p } {}
        void operator++() override final {
            ++ptr;
        }
        bool operator!=(Iterator<T>& rhs) override final {
            auto it = static_cast<DBIterator&>(rhs);
            return getPtr()!=it.getPtr();
        }
        bool operator==(Iterator<T>& rhs) override final {
            auto it = static_cast<DBIterator&>(rhs);
            return getPtr()==it.getPtr();
        }
        const T operator*() const override{
            assert(false); //이 메소드는 무조건 오버라이드 되어야 합니다.
            return T();
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
    QString DBType{"QSQLITE"};
    static QSqlDatabase db;
    QStringList column_names;
    QString file_name;
    QString connection_name;
private:

    template <typename T>
    static QSqlQuery bindValue(QSqlQuery query, T arg) {
        query.addBindValue(arg);
        return query;
    }
    template <typename T,typename... Types>
    static QSqlQuery bindValue(QSqlQuery query, T arg, Types... args) {
        query.addBindValue(arg);
        return bindValue(query, args...);
    }
    template <std::size_t sz>
    static QString appendQueryString(QString query){
        query+="?,";
        return appendQueryString<sz-1>(query);
    }
    template <>
    static QString appendQueryString<0>(QString query){
        query.chop(1);
        query+=")";
        return query;
    }
};

template <const char* table_name>
QSqlDatabase DBManager<table_name>::db {};

#endif // DBMANAGER_H
