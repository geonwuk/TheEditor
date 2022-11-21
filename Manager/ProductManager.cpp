#include "ProductManager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <random>
#include <functional>
using namespace std;
using namespace PM;

unsigned int ProductManager::product_id=0;

string ProductManager::generateRandID(tm time){
    static int inc=0;
    mt19937 engine;                    // MT19937 난수 엔진
    uniform_int_distribution<int> distribution(0, 10);
    auto generator = bind(distribution, engine);

    stringstream result;
    result <<char(65 + (generator()+product_id)%25)<<time.tm_hour<< time.tm_min <<time.tm_sec+inc++;
    return result.str();
}

std::ofstream& operator<< (std::ofstream& out, tm p) {
    out << std::put_time(&p, "%D %T");
	return out;
}

std::ofstream& PM::operator<<(std::ofstream& out, const Product& p){
	out << p.getId() << ',' << p.getName() << ',' << p.getPrice() << ',' << p.getQty()<<',';
    out << p.getDate();
	return out;
}


bool PM::operator== (const Product& p, const NoProduct&) { 
	const Product& np{ no_product };
	if (&p == &np)
		return true;
	else
		return false;
}

bool ProductManager::addProduct(const string name, const unsigned int price, const unsigned int qty)
{
    time_t base_time = time(nullptr);
    tm local_time;
    localtime_s(&local_time, &base_time);
    string ID = generateRandID(local_time);
    return loadProduct(ID, name, price, qty, local_time);
}

bool ProductManager::loadProduct(const string id, const string name, const unsigned int price, const unsigned int qty, tm time)
{
    bool success;
    tie(ignore, success) = products.emplace(id, Product(id, name, price, qty, time));
    product_id++;
    return success;
}

void ProductManager::loadProduct(const std::vector<PM::Product>& products_to_load){
    unsigned int line=0;
    for(const auto& p : products_to_load){
        if(!loadProduct(p.id,p.name,p.getPrice(),p.qty,p.registered_date))
            throw ERROR_WHILE_LOADING{line};
        ++line;
    }
}

void ProductManager::checkSafeToLoad(const vector<Product>& products_to_add) {
    unsigned int line=0;
    for(const auto& p : products_to_add){
        if(products.find(p.getId())!=products.end()){   //만약 파일(DB, CSV)로부터 추가하려는 상품 ID가 이미 중복된 경우 로딩 불가입니다
            throw ERROR_WHILE_LOADING{line};            //어느 라인에서 로딩하다가 에러가 났는지 예외를 던집니다.
        }
        ++line;
    }
}

bool ProductManager::modifyProduct(const PID id, const Product new_product){
    auto it = products.find(id);
    if (it == products.end()) return false;
    Product& the_product = it->second;
    the_product = new_product;
    return true;
}

bool ProductManager::eraseProduct(const PID id){
	using int_type = decltype(products)::size_type;
	int_type success = products.erase(id);
	if (success == 1)
		return true;
	else
		return false;
}

bool ProductManager::buyProduct(const PID id, const unsigned int qty) { //상품을 사는 경우 product의 qty(재고량)에서 감소시켜야 합니다
    auto it = products.find(id);
    Product& product= it->second;
    if (it == products.end()||product.getQty()<qty) {
        return false;
	}
	else {
        return product.decreaseQty(qty);
	}
}

const Product ProductManager::findProduct(const PID id) const{
    auto it = products.find(id);
    if (it == products.end()) {
        return no_product;
    }
    else {
        return it->second;
    }
}

const unsigned int ProductManager::getSize() const{
    return (unsigned int)products.size();
}
