#ifndef DB_CLIENTMANAGER_H
#define DB_CLIENTMANAGER_H

#include <string>
#include <vector>

#include <QString>
#include <QSqlDatabase>

#include "DB/DB_Manager.h"
#include "Model/Model.h"

extern const char CLIENT_TABLE_NAME[];
/*
ClientModel 인터페이스를 구현한 클래스로 ClientModel 인터페이스에 필요한 기능을 정의합니다
메모리 대신 DB를 씀으로써 데이터를 관리하며 사용자의 입력을 DB쿼리로 변환하여 DB를 조작합니다
*/
namespace DBM {//데이터베이스와 관련된 클래스가 속한 네임스페이스입니다
class ClientManager : public ClientModel, public DBManager<CLIENT_TABLE_NAME>
{
public:
    ClientManager(QString connection_name, QString file_name);
    const QSqlDatabase& getDb() {return db;}
    unsigned int getSize() const override;
    const CM::Client findClient(const CM::CID) const override;
    bool addClient(std::string, std::string, std::string, std::string) override;
    void loadClient(const std::vector<CM::Client>&) override;
    bool modifyClient(const CM::CID, const CM::Client ) override;
    bool eraseClient(const CM::CID) override;
    CM::Client copyClient(const CM::CID) const override;    //
    void checkSafeToLoad(const std::vector<CM::Client>&) noexcept(false) override;
    IteratorPTR<CM::Client> begin() const override;
    IteratorPTR<CM::Client> end() const override;
private:
    class CIterator : public DBIterator<CM::Client> {
    public:
        using DBIterator::DBIterator;
        const CM::Client operator*() const override ;
    };
};
} //namespace DBM
#endif // DB_CLIENTMANAGER_H
