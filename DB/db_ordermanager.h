#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H
#include "Model/model.h"
#include "DB/dbmanager.h"
extern const char ORDER_TABLE_NAME[];
namespace DBM {
class OrderManager : public OrderModel, public DBManager<ORDER_TABLE_NAME>
{
public:
    OrderManager(ClientModel& cm, ProductModel& pm, QString connection_name, QString file_name);
    OrderManager(ClientModel& cm, ProductModel& pm, QString connection_name);
    std::pair<const OM::Order_ID, bool> addOrder(const CM::CID client_id, std::vector<bill>) override;
    const OM::Order findOrder(const OM::Order_ID order_id) const override;
    const size_t getSize() const override;
    IteratorPTR<OM::Order> begin() override;
    IteratorPTR<OM::Order> end() override;
private:
    unsigned int order_id = 1;
    const ClientModel& cm;
    ProductModel& pm;
    void createTable(QString file_name);

public:
    struct OIterator : public DBIterator<OM::Order> {
    public:
        OIterator(Itr_type itr,DBM::OrderManager& om) : DBIterator<OM::Order>(itr), om{om} {}
        const OM::Order operator*() const override;

    private:
       // unsigned int order_id=2;
        DBM::OrderManager& om;
    };
};
}
#endif // ORDERMANAGER_H
