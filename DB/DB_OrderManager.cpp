#include "DB_OrderManager.h"

#include <ctime>
#include <sstream>

#include <QSqlQueryModel>
#include <QTableView>

using namespace DBM;
using namespace std;
extern const char ORDER_TABLE_NAME[]="Orders";

OrderManager::OrderManager(ClientModel& cm, ProductModel& pm, QString connection_name, QString file_name) : DBManager{connection_name, file_name}, cm{cm}, pm{pm} {
    initTable();
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
        if(product==PM::no_product){
            return {i, false};
        }
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
        ExplicitAdd explicit_add{db, "Orders"};         //DB에서 order_id는 primary key autoincrement이므로 primary 키 부분을 뺀 나머지 필드에 대해서 insert를 해야합니다
        QSqlQuery query = explicit_add.add({{"client_id",client_id.c_str()},{"price",QString::number(total_price)},{"date",ss.str().c_str()}});
        query.exec();
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
        query2.prepare("select * from Ordered_Product where product_id = :id order by product_history_seq desc");   //
        query2.bindValue(":id",id);     //상품 아이디를 바인드합니다
        query2.exec();                  //쿼리를 실행합니다
        query2.seek(0);                 //첫번째 레코드를 가져옵니다
        unsigned int siz = getQuerySize(query2);

        //qDebug()<<"ordered_product find "<<query2.lastQuery()<<query2.lastError();
        auto record = query2.record();

        auto n1 = record.value("name").toString()==name;                        //상품 중복 조건 첫번째
        //qDebug()<<"n1"<<n1;
        auto n2 = record.value("price").toString()==QString::number(price);     //상품 중복 조건 두번째
        //qDebug()<<"n2"<<n2;
        auto n3 = record.value("date").toString()==date;                        //상품 중복 조건 세번째
        //qDebug()<<"n3"<<n3;

        auto seq = record.value("product_history_seq").toUInt();
        if(siz==0 || !(n1&&n2&&n3)){                                                      //상품 조건 3가지가 모두 같지 않으면 새로운 상품이므로 새롭게 추가하고 seq를 증가시켜 새로운 상품으로 주문에 추가하도록 합니다
            auto query = add(QString("Ordered_Product"),id,++seq,name,price,date);
            query.exec();
            //qDebug()<<"Ordered_Product insert "<<query.lastQuery()<<query.lastError();
        }

        ExplicitAdd order_list{db,"order_list"};
        auto order_list_insert = order_list.add({{"order_id",QString::number(order_id)},{"product_id",id},
                                                 {"product_history_seq",QString::number(seq)},{"qty",QString::number(bill.qty)}});
        order_list_insert.exec();
        //qDebug()<<"order_list insert"<<order_list_insert.lastQuery()<<order_list_insert.lastError();

        pm.buyProduct(id.toStdString(),bill.qty);
    }

    return {0,true};
}
void OrderManager::loadOrder(const std::vector<OM::Order>& orders_to_add){
    for(const auto& o : orders_to_add){
        unsigned int order_id=0;
        {
            unsigned int total_price = 0;
            for(const auto& p : o.getProducts()){
                const auto& product = p.product;
                total_price += product.getPrice()*p.qty;
            }
            ExplicitAdd explicit_add{db, "Orders"};
            QSqlQuery query = explicit_add.add({{"client_id",o.getClient().getId().c_str()},{"price",QString::number(total_price)},{"date",tmToString(o.getDate())}});
            query.exec();
            order_id = query.lastInsertId().toUInt();
        }
        for(const auto& p : o.getProducts()){
            const auto& product = p.product;
            QString pid = product.getId().c_str();
            QString name = product.getName().c_str();
            auto price = product.getPrice();
            auto date = product.getDate();

            QSqlQuery ordered_product_query{db};
            ordered_product_query.prepare("select * from Ordered_Product where product_id = :id");
            ordered_product_query.bindValue(":id",pid);
            ordered_product_query.exec();
            unsigned int siz = getQuerySize(ordered_product_query);
            int seq=0;
            if(siz == 0){   //처음 추가하는 경우 무조건 추가해야 합니다
                add(QString("Ordered_Product"),pid,seq,name,price,tmToString(date)).exec();
            }
            else{
                while(ordered_product_query.next()){
                    auto n1 = ordered_product_query.value("name").toString()==name;                        //상품 중복 조건 첫번째
                    auto n2 = ordered_product_query.value("price").toString()==QString::number(price);     //상품 중복 조건 두번째
                    auto n3 = ordered_product_query.value("date").toString()==tmToString(date);            //상품 중복 조건 세번째
                    if((n1&&n2&&n3)){
                        goto found; //이미 ordered_produdct table에 데이터가 저장되어 있으므로 추가를 할 필요가 없다는 의미입니다
                    }
                    ++seq;
                }
                //while을 끝까지 돌았다는 의미는 새로 추가해줘야 한다는 의미입니다.
                add(QString("Ordered_Product"),pid,seq,name,price,tmToString(date)).exec();
            }
            found:
            ExplicitAdd order_list{db,"order_list"};
            auto order_list_insert = order_list.add({{"order_id",QString::number(order_id)},{"product_id",pid},
                                                     {"product_history_seq",QString::number(seq)},{"qty",QString::number(p.qty)}});
            order_list_insert.exec();
        }
    }

}
const OM::Order OrderManager::findOrder(const OM::Order_ID oid) const{
    QSqlQuery query{db};
    query.prepare("select * from orders o, order_list ol where o.id=:order_id and o.id = ol.order_id");
    query.bindValue(":order_id",oid);
    query.exec();

    unsigned int siz =0;
    while(query.next()){    //sqlite는 size()함수를 지원하지 않아서 직접 세어야 합니다.
        ++siz;
    }
    if(siz == 0)
        return OM::no_order;
    query.seek(0);          //개수를 다 센 후 다시 첫번째 레코드로

    QString order_date = query.value("date").toString();
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
        find_query.exec();
        find_query.next();
        auto name = find_query.value("name").toString().toStdString();
        auto price =find_query.value("price").toUInt();
        QString date = find_query.value("date").toString();

        //qDebug()<<"product"<< name.c_str()<<price<<date<<qty;
        PM::Product p {pid,name,price,qty,DBManager::getDate(date)};
        products.emplace_back(p,qty);
    }
    return OM::Order{oid,client,DBManager::getDate(order_date),std::move(products)};
}
const size_t OrderManager::getSize() const{
    auto query = DBManager::getSize();
    query.exec();
    return query.next() ? query.value(0).toUInt() : 0;
}
void OrderManager::checkSafeToLoad(const std::vector<OM::Order>& orders_to_add) {
    int line=0;
    for(const auto& o : orders_to_add ){
        if(findOrder(o.getID())==OM::no_order){
            throw ERROR_WHILE_LOADING{line};
        }
        ++line;
    }
}
IteratorPTR<OM::Order> OrderManager::begin(){
    return IteratorPTR<OM::Order>{ new OIterator{0,*this,db} };       //쿼리문의 첫번째 index는 0부터 시작하므로 0으로 초기화합니다
}
IteratorPTR<OM::Order> OrderManager::end(){
    QSqlQuery query{QString("select count(id) from ") + ORDER_TABLE_NAME, db};              //id 레코드 개수를 알기 위한 쿼리를 실행합니다
    if (query.next()) {
        return IteratorPTR<OM::Order>{new OIterator{query.value(0).toInt(),*this,db}};        //레코드 개수 만큼 end로 설정합니다
    } else {
        return IteratorPTR<OM::Order>{new OIterator{0,*this,db}};                             //만약 쿼리 결과 레코드가 하나도 없다면 0입니다
    }
}
void OrderManager::createTable(QString create_query){           //주문의 경우 DB 테이블이 3개가 필요해서 테이블을 만드는 함수를 만들었습니다
    auto query_result = db.exec(create_query);
    if(query_result.lastError().isValid())
        throw DBM::ERROR_WHILE_LOADING{create_query.toStdString()};
}
void OrderManager::initTable(){                                     //DB에서 테이블 3개를 만드는 작업을 합니다
    QString create_query = "Create TABLE IF NOT EXISTS Orders ("
                           "id INTEGER PRIMARY KEY,"
                           "client_id varchar(20),"
                           "price INTEGER,"
                           "date TEXT,"
                           "foreign key(client_id) references client(id));";

    QString create_order_list = "CREATE TABLE IF NOT EXISTS ORDER_LIST ("
                                "SEQ INTEGER PRIMARY KEY autoincrement,"
                                "order_id INTEGER,"
                                "product_id varchar(20),"
                                "product_history_seq INTEGER,"
                                "qty INTEGER,"
                                "foreign key(product_id) references Ordered_product(id),"
                                "foreign key(product_history_seq) references Ordered_product(product_history_seq));";

    QString create_ordered_product = "CREATE TABLE IF NOT EXISTS Ordered_Product ("
                                     "product_id varchar(20),"
                                     "product_history_seq INTEGER,"
                                     "name varchar(20),"
                                     "price INTEGER,"
                                     "date TEXT,"
                                     "foreign key(product_id) references Product(id));";
    createTable(create_query);
    createTable(create_order_list);
    createTable(create_ordered_product);
    auto query_result = db.exec(create_query);
    if(query_result.lastError().isValid())
        throw DBM::ERROR_WHILE_LOADING{"Product"};
    QSqlQuery columnNamesQuery {QString("SELECT name FROM PRAGMA_TABLE_INFO('")+ORDER_TABLE_NAME+"');",db};
    while(columnNamesQuery.next()){
        column_names<<columnNamesQuery.value(0).toString();
    }
    column_names.removeFirst();//ID 칼럼 삭제
}
const OM::Order OrderManager::OIterator::operator*() const {
    const QSqlRecord record = getPtr();                     //현재 iterator가 가리키는 레코드를 획득합니다
    return om.findOrder(record.value("id").toUInt());       //iteraotr가 가리키는 주문을 리턴합니다
}
