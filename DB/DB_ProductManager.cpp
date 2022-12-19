#include "DB_ProductManager.h"

#include <random>
#include <sstream>

extern const char PRODUCT_TABLE_NAME[]="Product";
using namespace DBM;
using namespace std;
using std::string;

unsigned int ProductManager::product_id=0;

template<typename T>
static PM::Product makeProductFromDB(T record){
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
ProductManager::ProductManager(QString connection_name, QString file_name):DBManager{connection_name, file_name}{
    QString create_query = "Create TABLE IF NOT EXISTS Product("
                            "id varchar(20) PRIMARY KEY,"
                            "name varchar(20),"
                            "price INTEGER,"
                            "qty INTEGER,"
                            "date TEXT);";
    auto query_result = db.exec(create_query);
    if(query_result.lastError().isValid())
        throw DBM::ERROR_WHILE_LOADING{"Product"};
    QSqlQuery columnNamesQuery {QString("SELECT name FROM PRAGMA_TABLE_INFO('")+PRODUCT_TABLE_NAME+"');",db};
    while(columnNamesQuery.next()){
        column_names<<columnNamesQuery.value(0).toString();
    }
    column_names.removeFirst();//ID 칼럼 삭제
}
bool ProductManager::addProduct(const std::string name, const unsigned int price, const unsigned int qty) {
    time_t base_time = time(nullptr);
    tm local_time;
    localtime_s(&local_time, &base_time);
    string id = generateRandID(local_time);
    return loadProduct(id,name,price,qty,local_time);
}
void ProductManager::checkSafeToLoad(const std::vector<PM::Product>& products_to_add){
    unsigned int line=0;
    for(const auto& p : products_to_add){
        if(findProduct(p.getId())==PM::no_product){     //만약 파일(DB, CSV)로부터 추가하려는 상품 ID가 이미 중복된 경우 로딩 불가입니다
            throw ERROR_WHILE_LOADING{line};            //어느 라인에서 로딩하다가 에러가 났는지 예외를 던집니다.
        }
        ++line;
    }
}
void ProductManager::loadProduct(const std::vector<PM::Product>& products_to_add){
    unsigned int line=0;
    for(const auto& p : products_to_add){
        bool re = loadProduct(p.getId(),p.getName(),p.getPrice(),p.getQty(),p.getDate());
        if(!re)
            throw ERROR_WHILE_LOADING{line};
        ++line;
    }
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
    return IteratorPTR<PM::Product>{ new PIterator{0,db} };
}
IteratorPTR<PM::Product> ProductManager::end() {
  QSqlQuery query{QString("select count(id) from ") + PRODUCT_TABLE_NAME, db};
  if (query.next()) {
    return IteratorPTR<PM::Product>{new PIterator{query.value(0).toInt(),db}};
  } else {
    return IteratorPTR<PM::Product>{new PIterator{0,db}};
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
