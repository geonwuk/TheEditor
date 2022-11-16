#include "db_productmanager.h"
#include <ctime>
#include <sstream>
#include <random>
extern const char PRODUCT_TABLE_NAME[]="Product";
using namespace DBM;
using namespace std;
using std::string;

unsigned int ProductManager::product_id=0;

template<typename T>
PM::Product makeProductFromDB(T record){
    string id = record.value("id").toString().toStdString();
    string name = record.value("name").toString().toStdString();
    unsigned int price = record.value("price").toString().toUInt();
    unsigned int qty = record.value("qty").toString().toUInt();
    string date = record.value("date").toString().toStdString();
    tm time;
    std::istringstream ss{ date };
    ss >> std::get_time(&time, "%D %T");
    return PM::Product(id,name,price,qty,time);
}

bool ProductManager::addProduct(const std::string name, const unsigned int price, const unsigned int qty) {
    time_t base_time = time(nullptr);
    tm local_time;
    localtime_s(&local_time, &base_time);
    string id = generateRandID(local_time);
    return ProductManager::loadProduct(id,name,price,qty,local_time);
}

bool ProductManager::loadProduct(const std::string id, const std::string name, const unsigned int price, const unsigned int qty, std::tm time){
    ostringstream ss;
    ss<<put_time(&time, "%D %T");
    auto query = add(id.c_str(), name.c_str(), price, qty, ss.str().c_str());
    return query.exec();
}

bool ProductManager::modifyProduct(const PM::PID id, const PM::Product product){
    tm time = product.getDate();
    ostringstream ss;
    ss<<put_time(&time, "%D %T");
    auto query = modify(id.c_str(), product.getName().c_str(), product.getPrice(), product.getQty(), ss.str().c_str());
    qDebug()<<query.lastQuery();
    return query.exec();
}

bool ProductManager::eraseProduct(const PM::PID id) {
    auto query = erase(id.c_str());
    return query.exec();
}

const PM::Product ProductManager::findProduct(const PM::PID id) const {
    auto query = find(id.c_str());
    query.exec();
    if(query.next()){
        return makeProductFromDB(std::move(query));
    }
    else{
        return PM::no_product;
    }
}

bool ProductManager::buyProduct(const PM::PID id, const unsigned int qty) {
    auto query = find(id.c_str());
    query.exec();
    if(!query.next()){
        return false;
    }
    auto record = query.record();
    unsigned int stock = record.value("qty").toUInt();
    if(qty>stock){
        return false;
    }
    QSqlQuery update_query{db};
    update_query.prepare({"update product set qty = :qty where id = :id "});
    update_query.bindValue(":qty",stock-qty);
    update_query.bindValue(":id", id.c_str());
    return update_query.exec();
}

const unsigned int ProductManager::getSize() const {
    auto query = DBManager::getSize();
    query.exec();
    return query.next() ? query.value(0).toUInt() : 0;
}

IteratorPTR<PM::Product> ProductManager::begin(){
    return IteratorPTR<PM::Product>{ new PIterator{0} };
}
IteratorPTR<PM::Product> ProductManager::end() {
  QSqlQuery query{QString("select count(id) from ") + PRODUCT_TABLE_NAME, db};
  if (query.next()) {
    return IteratorPTR<PM::Product>{new PIterator{query.value(0).toInt()}};
  } else {
    return IteratorPTR<PM::Product>{new PIterator{0}};
  }
}





const PM::Product ProductManager::PIterator::operator*() const {
    const QSqlRecord record = getPtr();
    return makeProductFromDB(record);
}


string ProductManager::generateRandID(tm time){
    static int inc=0;
    mt19937 engine;                    // MT19937 난수 엔진
    engine.seed(time.tm_sec);
    uniform_int_distribution<int> distribution(0, 10);
    auto generator = bind(distribution, engine);
    stringstream result;
    result <<char(65 + (generator()+product_id)%25)<<time.tm_hour<< time.tm_min <<time.tm_sec+inc++;
    return result.str();
}
