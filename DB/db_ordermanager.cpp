#include "db_ordermanager.h"
#include <sstream>

using namespace DBM;
using namespace std;
extern const char ORDER_TABLE_NAME[]="Orders";

void OrderManager::createTable(QString file_name){
    QFile create_query_file{file_name};
    if(!create_query_file.open(QIODevice::ReadOnly|QIODevice::Text))
        throw -1;// todo
    QString create_query = create_query_file.readAll();
    create_query.remove('\n');
    qDebug()<<create_query;
    auto query_result = db.exec(create_query);
    if(query_result.lastError().isValid())
        throw -1;
}

OrderManager::OrderManager(ClientModel& cm, ProductModel& pm, QString connection_name, QString file_name) : DBManager{connection_name, file_name}, cm{cm}, pm{pm} {
    createTable(":/DB/Queries/createOrderListTable.txt");
    createTable(":/DB/Queries/createOrderedProductTable.txt");
}

OrderManager::OrderManager(ClientModel& cm, ProductModel& pm, QString connection_name) : DBManager(connection_name), cm{cm}, pm{pm}{
    createTable(":/DB/Queries/createOrderListTable.txt");
    createTable(":/DB/Queries/createOrderedProductTable.txt");


//    QSqlQuery query{"select id from Orders order by id desc;", db};
//    assert(query.exec());
//    query.next();
//    order_id+=query.value("id").toUInt();
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

    unsigned int order_id=0;
    {
        unsigned int total_price = 0;
        for(const auto& bill : bills){
            const auto& product = pm.findProduct(bill.id);
            total_price += product.getPrice()*bill.qty;
        }
        ExplicitAdd explicit_add{"Orders"};
        QSqlQuery query = explicit_add.add({{"client_id",client_id.c_str()},{"price",QString::number(total_price)},{"date",ss.str().c_str()}});
        assert(query.exec());
        order_id = query.lastInsertId().toUInt();
    }

    for(const auto& bill : bills){
        const auto& product = pm.findProduct(bill.id);
        QString id = bill.id.c_str();
        QString name = product.getName().c_str();
        auto price = product.getPrice();
        ostringstream ss;
        auto tm = product.getDate();
        ss<<put_time(&tm, "%D %T");
        QString date = ss.str().c_str();

        QSqlQuery query2{db};
        query2.prepare("select * from Ordered_Product where product_id = :id order by product_history_seq desc");
        query2.bindValue(":id",id);
        query2.exec();
        query2.seek(0);

        qDebug()<<"ordered_product find "<<query2.lastQuery()<<query2.lastError();
        auto record = query2.record();

        auto n1 = record.value("name").toString()==name;
        qDebug()<<"n1"<<n1;
        auto n2 = record.value("price").toString()==QString::number(price);
        qDebug()<<"n2"<<n2;
        auto n3 = record.value("date").toString()==date;
        qDebug()<<"n3"<<n3;

        auto seq = record.value("product_history_seq").toUInt();
        if(!(n1&&n2&&n3)){
            auto query = add(QString("Ordered_Product"),id,++seq,name,price,date);
            assert(query.exec());
            qDebug()<<"Ordered_Product insert "<<query.lastQuery()<<query.lastError();
        }

        ExplicitAdd order_list{"order_list"};
        auto order_list_insert = order_list.add({{"order_id",QString::number(order_id)},{"product_id",id},
                        {"product_history_seq",QString::number(seq)},{"qty",QString::number(bill.qty)}});
        assert(order_list_insert.exec());
        qDebug()<<"order_list insert"<<order_list_insert.lastQuery()<<order_list_insert.lastError();

        pm.buyProduct(id.toStdString(),bill.qty);
    }

    return {0,true};
}
#include <QSqlQueryModel>
#include <QTableView>
const OM::Order OrderManager::findOrder(const OM::Order_ID oid) const{
    QSqlQuery query{db};
    query.prepare("select * from orders o, order_list ol where o.id=:order_id and o.id = ol.order_id");
    query.bindValue(":order_id",oid);
    assert(query.exec());

    unsigned int siz =0;
    while(query.next()){ //sqlite는 size()함수를 지원하지 않습니다
        ++siz;
    }

    query.seek(0);
    qDebug()<<"size"<<siz;


    qDebug()<<"tt";
    QString order_date = query.value("date").toString();
    qDebug()<<"date"<<order_date;

    qDebug()<<"find orders join"<<query.lastQuery()<<query.lastError();





    auto cid = query.value("client_id").toString().toStdString();
    auto client = cm.findClient(cid);


    query.seek(-1);
    vector<std::pair<PM::Product,OM::Order::qty>> products;
    while(query.next()){
        auto pid = query.value("product_id").toString().toStdString();
        auto seq = query.value("product_history_seq").toUInt();
        OM::Order::qty qty = query.value("qty").toUInt();
        QSqlQuery find_query{db};
        find_query.prepare("select * from Ordered_Product where product_id=:pid and product_history_seq=:seq;");
        find_query.bindValue(":pid",pid.c_str());
        find_query.bindValue(":seq",seq);
        assert(find_query.exec());
        find_query.next();
        auto name = find_query.value("name").toString().toStdString();
        auto price =find_query.value("price").toUInt();
        QString date = find_query.value("date").toString();

        qDebug()<<"product"<< name.c_str()<<price<<date<<qty;
        PM::Product p {pid,name,price,qty,DBManager::getDate(date)};
        products.emplace_back(p,qty);
    }

//    qDebug()<<"oid"<<oid;
//    qDebug()<<"client"<<client.getId().c_str();
//    for(auto e: products){
//        qDebug()<<"product"<<e.first.getId().c_str()<<e.first.getName().c_str();
//    }

//    QSqlQueryModel* queryModel = new QSqlQueryModel;
//    queryModel->setQuery((query));
//    QTableView* tableView = new QTableView;
//    tableView->setModel(queryModel);
//    tableView->setWindowTitle(QObject::tr("Query Model"));
//    tableView->show();



    return OM::Order{oid,client,DBManager::getDate(order_date),std::move(products)};
}
const size_t OrderManager::getSize() const{
    auto query = DBManager::getSize();
    query.exec();
    return query.next() ? query.value(0).toUInt() : 0;
}
IteratorPTR<OM::Order> OrderManager::begin(){
    return IteratorPTR<OM::Order>{ new OIterator{0, *this} };
}

IteratorPTR<OM::Order> OrderManager::end(){
    QSqlQuery query{QString("select count(id) from ") + ORDER_TABLE_NAME, db};
    if (query.next()) {
      return IteratorPTR<OM::Order>{new OIterator{query.value(0).toInt(), *this}};
    } else {
      return IteratorPTR<OM::Order>{new OIterator{0, *this}};
    }
}


const OM::Order OrderManager::OIterator::operator*() const {
    const QSqlRecord record = getPtr();
    return om.findOrder(record.value("id").toUInt());
}
