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
namespace DBM {
struct ERROR_WHILE_LOADING{
    std::string db_name;
};

//Manager클래스에서 DB에 필요한 쿼리를 제공하는 클래스 입니다. 기본적인 데이터 추가,삭제,조회,크기 등 Manager에서 필요한 기능을 제공합니다
template<const char* table_name>
class DBManager
{
public:
    DBManager(QString connection_name, QString file_name) : file_name{file_name}, connection_name{connection_name}{
        DBManager::db = QSqlDatabase::addDatabase(DBType,connection_name);
        db.setDatabaseName(file_name);
        if(!db.open()){
            qDebug()<<"not open";
            db.close();
            db=QSqlDatabase();
            QSqlDatabase::removeDatabase(connection_name);
            throw ERROR_WHILE_LOADING{"NOT OPEN"};
        }
        //QSqlQuery countColumn {QString("SELECT count(*) FROM PRAGMA_TABLE_INFO('")+table_name+"')",db};
        //int num_of_columns = countColumn.next() ? countColumn.value(0).toInt() : 0;
    }
    ~DBManager(){
        db.close();
        db=QSqlDatabase();
        QSqlDatabase::removeDatabase(connection_name);
    }
    QSqlQuery getSize() const{
        return QSqlQuery{QString("select count(id) from ")+QString(table_name),db}; //Client, Product, Order테이블 모두 id필드를 갖으므로 id개수로 사이즈를 알 수 있습니다
    }
    QSqlQuery find(const QString id) const {    //ID로
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
    static QString tmToString(std::tm dt){
        std::ostringstream ss;
        ss<<std::put_time(&dt, "%D %T");
        return ss.str().c_str();
    }
    struct ExplicitAdd {
        ExplicitAdd(QSqlDatabase& db, QString tb_name) : tb_name{tb_name}, db{db} {}
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
        QSqlDatabase& db;
    };
    template<typename T>
    struct DBIterator : public Iterator<T>{
        using Itr_type = int;
        DBIterator(Itr_type p, const QSqlDatabase& db) : ptr{ p }, db{db} {}
        void operator++() override final {
            ++ptr;                                          //레코드 인덱스를 증가시킵니다
        }
        bool operator!=(Iterator<T>& rhs) override final {
            auto it = static_cast<DBIterator&>(rhs);
            return getPtr()!=it.getPtr();                   //서로 다른 레코드를 가리키는지 결과를 리턴합니다
        }
        bool operator==(Iterator<T>& rhs) override final {
            auto it = static_cast<DBIterator&>(rhs);
            return getPtr()==it.getPtr();                   //서로 같은 레코드를 가리키는지 결과를 리턴합니다
        }
        const T operator*() const override{
            assert(false);                  //이 메소드는 무조건 오버라이드 되어야 합니다.
            return T();
        }
        QSqlRecord getPtr() const{
            QSqlQuery query{QString("select * from ")+table_name+" order by id;",db};       //현재 DB가 가리키는 레코드를 리턴합니다
            query.seek(ptr);
            return query.record();
        }
    private:
        Itr_type ptr;
        const QSqlDatabase& db;
    };

protected:
    QString DBType{"QSQLITE"};
    QSqlDatabase db;
    QStringList column_names;
    QString file_name;
    QString connection_name;

    unsigned int getQuerySize(QSqlQuery& query){
        unsigned int siz =0;
        auto saved = query.at();
        query.seek(-1);
        while(query.next()){    //sqlite는 size()함수를 지원하지 않아서 직접 세어야 합니다.
            ++siz;
        }
        query.seek(saved);          //개수를 다 센 후 다시 이전 레코드로
        return siz;
    }
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

}

#endif // DBMANAGER_H
