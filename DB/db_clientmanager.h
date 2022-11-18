#ifndef DB_CLIENTMANAGER_H
#define DB_CLIENTMANAGER_H

#include <QString>
#include "Model/model.h"
#include <QSqlDatabase>
#include "DB/dbmanager.h"
extern const char name2[];
/*
ClientModel 인터페이스를 구현한 클래스로 ClientModel 인터페이스에 필요한 기능을 정의합니다
메모리 대신 DB를 씀으로써 데이터를 관리하며 사용자의 입력을 DB쿼리로 변환하여 DB를 조작합니다
*/
namespace DBM {//데이터베이스와 관련된 클래스가 속한 네임스페이스입니다
class ClientManager : public ClientModel, public DBManager<name2>{
public:
    ClientManager(QString connection_name);
    ClientManager(QString connection_name, QString file_name):DBManager{connection_name,file_name} {}
    const QSqlDatabase& getDb() {return db;}
    unsigned int getSize() const override;
    const CM::Client findClient(const CM::CID) const override;
    bool addClient(std::string, std::string, std::string, std::string) override;
    bool modifyClient(const CM::CID, const CM::Client ) override;
    bool eraseClient(const CM::CID) override;
    CM::Client copyClient(const CM::CID) const override;    //
    IteratorPTR<CM::Client> begin() const override;
    IteratorPTR<CM::Client> end() const override;
private:
    class CIterator : public DBIterator<CM::Client> {
    public:
        using DBIterator::DBIterator;
        const CM::Client operator*() const override ;
    };
};

}
#endif // DB_CLIENTMANAGER_H
