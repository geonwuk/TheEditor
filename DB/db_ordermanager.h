#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <cuchar>

#include <string>
#include <utility>
#include <vector>

#include <QString>

#include "DB/dbmanager.h"
#include "Model/model.h"

extern const char ORDER_TABLE_NAME[];
/*
OrderModel 인터페이스를 구현한 클래스로 OrderModel 인터페이스에 필요한 기능을 정의합니다
메모리 대신 DB를 씀으로써 데이터를 관리하며 사용자의 입력을 DB쿼리로 변환하여 DB를 조작합니다
*/
namespace DBM {//데이터베이스와 관련된 클래스가 속한 네임스페이스입니다
class OrderManager : public OrderModel, public DBManager<ORDER_TABLE_NAME>
{
public:
    OrderManager(ClientModel& cm, ProductModel& pm, QString connection_name, QString file_name);
    std::pair<const OM::Order_ID, bool> addOrder(const CM::CID client_id, std::vector<bill>) override;
    void loadOrder(const std::vector<OM::Order>&) noexcept(false) override;
    const OM::Order findOrder(const OM::Order_ID order_id) const override;
    const size_t getSize() const override;
    void checkSafeToLoad(const std::vector<OM::Order>&) noexcept(false) override;
    IteratorPTR<OM::Order> begin() override;
    IteratorPTR<OM::Order> end() override;
private:
    const ClientModel& cm;
    ProductModel& pm;
    void createTable(QString file_name);
    void initTable();
public:
    struct OIterator : public DBIterator<OM::Order> {
    public:
        OIterator(Itr_type itr, DBM::OrderManager& om, const QSqlDatabase& db) : DBIterator<OM::Order>(itr,db), om{om} {}
        const OM::Order operator*() const override;
    private:
        DBM::OrderManager& om;
    };
};
} //namespace DBM
#endif // ORDERMANAGER_H
