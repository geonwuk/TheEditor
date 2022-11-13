#include "db_ordermanager.h"
#include <sstream>

using namespace DBM;
using namespace std;
extern const char ORDER_TABLE_NAME[]="Ordres";
OrderManager::OrderManager(ClientModel& cm, ProductModel& pm) : DBManager(), cm{cm}, pm{pm}{

    QSqlQuery columnNamesQuery {QString("SELECT name FROM PRAGMA_TABLE_INFO('")+table_name+"');",db};
    while(columnNamesQuery.next()){
        qDebug()<<columnNamesQuery.value(0).toString();
        column_names<<columnNamesQuery.value(0).toString();
    }
    column_names.removeFirst();//ID 칼럼 삭제
}



std::pair<const OM::Order_ID, bool> OrderManager::addOrder(const CM::CID client_id, std::vector<bill> bills){
    time_t base_time = time(nullptr);
    tm local_time;
    localtime_s(&local_time, &base_time);
    ostringstream ss;
    ss<<put_time(&local_time, "%D %T");

    int i=0;
    for(const auto& bill : bills){                      //모든 주문에 대해 재고량이 주문량 보다 많은지 확인하고 만약 적다면 주문자체를 취소합니다
        const auto product = pm.findProduct(bill.id);
        assert(!(product==PM::no_product));
        if(product.getQty()<bill.qty){
            return {i++, false};
        }
    }

    for(const auto& bill : bills){
        const auto product = pm.findProduct(bill.id);
        QString id = bill.id.c_str();
        QString name = product.getName().c_str();
        auto price = product.getPrice();
        ostringstream ss;
        auto tm = product.getDate();
        ss<<put_time(&tm, "%D %T");
        QString date = ss.str().c_str();

        unsigned int total_price = price*bill.qty;
        ExplicitAdd explicit_add{"Orders"};
        QSqlQuery query = explicit_add.add({{"client_id",client_id.c_str()},{"price",QString::number(total_price)},{"date",ss.str().c_str()}});
        assert(query.exec());

        auto order_id = query.lastInsertId().toUInt();

        QSqlQuery query2{db};
        query2.prepare("select * from Ordered_Product where product_id = :id order by product_history_seq desc");
        query2.bindValue(":id",id);
        query2.exec();
        query2.seek(0);
        auto record = query2.record();

        auto n1 = record.value("name").toString()==name;
        auto n2 = record.value("price").toString()==QString::number(price);
        auto n3 = record.value("date").toString()==date;

        auto seq = record.value("product_history_seq").toUInt();
        if(!(n1&&n2&&n3)){
            auto query = add(QString("Ordered_Product"),id,++seq,name,price,date);
            assert(query.exec());
        }

        ExplicitAdd order_list{"order_list"};
        auto order_list_insert = order_list.add({{"order_id",QString::number(order_id)},{"product_id",id},
                        {"product_history_seq",QString::number(seq)},{"qty",QString::number(bill.qty)}});
        assert(order_list_insert.exec());

        pm.buyProduct(id.toStdString(),bill.qty);
    }

}
const OM::Order& OrderManager::findOrder(const OM::Order_ID id) const{
    QSqlQuery query{db};
    query.prepare("select * from order")
    query.exec();
    if(query.next()){
        OM::Order order;

        //return makeProductFromDB(std::move(query));
    }
    else{
        return OM::no_order;
    }
}
const size_t OrderManager::getSize() const{
    auto query = DBManager::getSize();
    query.exec();
    return query.next() ? query.value(0).toUInt() : 0;
}
IteratorPTR<OM::Order> OrderManager::begin(){
    return IteratorPTR<OM::Order>{ new OIterator{0} };
}

IteratorPTR<OM::Order> OrderManager::end(){
    QSqlQuery query{QString("select count(id) from ") + ORDER_TABLE_NAME, db};
    if (query.next()) {
      return IteratorPTR<OM::Order>{new OIterator{query.value(0).toInt()}};
    } else {
      return IteratorPTR<OM::Order>{new OIterator{0}};
    }
}


const OM::Order OrderManager::OIterator::operator*() const {
    const QSqlRecord record = getPtr();
    return OM::Order();
}
