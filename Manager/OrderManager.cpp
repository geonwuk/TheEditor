
#include "OrderManager.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cassert>
#include "Manager/ProductManager.h"
using namespace std;
using namespace OM;
using namespace PM;

std::pair<const Order_ID, bool> OrderManager::addOrder(const Client_ID client_id, vector<bill> products){
    time_t base_time = time(nullptr);
    tm local_time;
    localtime_s(&local_time, &base_time);
    return addOrder(order_id, client_id, products, local_time);
}

std::pair<const Order_ID, bool> OrderManager::addOrder(const Order_ID oid, const Client_ID client_id, vector<bill> products, tm time){
    if(products.empty()){
        return {0, false};
    }
    Order order;
    order.date = time;

    for (const auto &product : products) {              //vector<bill> 주문서에 있는 구매하고자 하는 상품 목록을 모두 검사합니다.
        Product found = pm.findProduct(product.id);     //bill 주문서에 있는 상품을 찾습니다
        if (found == no_product)
            return { oid, false };                      //만약의 경우에 상품을 못 찾는 경우 거짓을 리턴합니다
        if (found.getQty()<product.qty)                 //상품 재고량 보다 구매하고자 하는 개수가 많은 경우 구매할 수 없습니다
            return { oid, false };
    }
    int i=0;
    for (auto product : products) {                                                 //구매할 물건에 대해
        order.products.emplace_back(pm.findProduct(product.id), product.qty);       //주문에 구매한 상품들을 추가합니다
        assert(pm.buyProduct(product.id, product.qty));                             //GUI 상에서 주문 가능한 물품만 선택할 수 있도록 하였으므로 못 구매하는 것은 프로그램 오류입니다
        i++;
    }
    order.order_id = oid;                                   //오더에 새로 추가할 id를 입력합니다
    assert(!(cm.findClient(client_id)==no_client));         //GUI 상에서 주문 가능한 고객만 선택할 수 있도록 하였으므로 못 찾는 것은 프로그램 오류입니다
    order.client = cm.findClient(client_id);                //고객ID로 고객정보를 추출합니다
    auto inserted_order = orders.emplace(order_id, order);  //주문 목록에 주문을 추가합니다
    return {order_id++, true};                              //오더 카운트 증가를 한 후 리턴합니다
}

bool OrderManager::loadOrder(const Order_ID oid, const Client_ID client_id, vector<Product> products, vector<unsigned int> qty, tm time){
    Order order;
    order.date = time;

    unsigned int i =0;
    for (auto product : products) {
        order.products.emplace_back(product, qty.at(i++));
    }
    order.order_id = oid;
    if(!(cm.findClient(client_id)==no_client)){                 //
        throw ERROR_WHILE_LOADING{oid};
    }
    order.client = cm.findClient(client_id);
    orders.emplace(order_id, order);
    order_id = (oid>order_id ? oid : order_id);
    order_id++;
    return true;
}

const Order OrderManager::findOrder(const Order_ID order_id) const {
	auto o = orders.find(order_id);
	if (o == orders.end()) {
		return no_order;
	}
	else {
		return o->second;
    }
}
static std::ofstream& operator<< (std::ofstream& out, tm p) {
    out << std::put_time(&p, "%D %T");
    return out;
}


std::ofstream& OM::operator<<(std::ofstream& out, const Order& order)
{
    out << order.getID() << ',';
    out << order.getClient().getId() << ',';
    out << order.getDate()<<',';
    out << order.getProducts().size()<<','<<endl;
    for(const auto& product_info : order.getProducts()){
        out<<product_info.product<<','<<product_info.qty;
    }
	return out;
}
static vector<string> split(const string& str){
    vector<string> tmp;
    auto begIdx = str.find_first_not_of(',');
    while (begIdx != string::npos) {
        auto endIdx = str.find_first_of(',', begIdx);
        if (endIdx == string::npos) {
            endIdx = str.length();
        }
        tmp.emplace_back(str.substr(begIdx, endIdx - begIdx));
        begIdx = str.find_first_not_of(',', endIdx);
    }
    return tmp;
}

std::ifstream& OM::OrderManager::loadOrders(ifstream& in, unsigned int lines)
{
    unsigned int line=0;
    try{
        std::string str;
        while (line++<lines && getline(in, str)) {
            vector tmp = split(str);
            Order_ID oid = stoi(tmp.at(0));
            Client_ID cid = tmp.at(1);
            string time_string = tmp.at(2);
            std::tm time;
            istringstream ss{ time_string };
            if(ss.fail())
                throw -1;
            ss >> std::get_time(&time, "%D %T");

            unsigned int products_size = stoi(tmp.at(3));
            vector<Product> products;
            vector<unsigned int> qtys;
            for(unsigned int i=0; i<products_size; i++){
                string product;
                getline(in,product);
                vector product_string= split(product);
                string time_string = product_string.at(4);
                tm time;
                istringstream ss{ time_string };
                ss >> std::get_time(&time, "%D %T");
                unsigned int stock = stoul(product_string.at(3));
                unsigned int price = stoul(product_string.at(2));
                string name = product_string.at(1);
                string id = product_string.at(0);
                products.emplace_back(id,name,price,stock,time);
                qtys.emplace_back(stoi(product_string.at(5)));
            }
            auto result = loadOrder(oid, cid, products, qtys, time);
            if(!result){
                throw -1;
            }
        }
        return  in;
    }
    catch(...){
        throw ERROR_WHILE_LOADING{line};
    }
}





IteratorPTR<OM::Order> OrderManager::begin(){
    return IteratorPTR<Order>(new OIterator{orders.begin()});       //map의 begin을 IteratorPTR에 감쌉니다.
}
IteratorPTR<OM::Order> OrderManager::end(){
     return IteratorPTR<Order>(new OIterator{orders.end()});        //map의 end를 IteratorPTR에 감쌉니다.
}
const Order OrderManager::OIterator::operator*() const {
    return ptr->second;                                             //
}
void OrderManager::OIterator::operator++(){
    ++ptr;
}
bool OrderManager::OIterator::operator!=(Iterator& rhs){
    return (ptr != static_cast<OIterator&>(rhs).ptr ? true : false);
}
bool OrderManager::OIterator::operator==(Iterator& rhs){
    return (ptr == static_cast<OIterator&>(rhs).ptr ? true : false);
}
