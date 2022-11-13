
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

    for (const auto &product : products) {
        Product found = pm.findProduct(product.id);
        if (found == no_product)
            return { oid, false };
        if (found.getQty()<product.qty)
            return { oid, false };
    }
    int i=0;
    for (auto product : products) {
        order.products.emplace_back(pm.findProduct(product.id), product.qty);
        assert(pm.buyProduct(product.id, product.qty));
        i++;
    }
    order.order_id = oid;
    assert(!(cm.findClient(client_id)==no_client));
    order.client = cm.findClient(client_id);
    auto inserted_order = orders.emplace(order_id, order);
    return {order_id++, true};
}

bool OrderManager::loadOrder(const Order_ID oid, const Client_ID client_id, vector<Product> products, vector<unsigned int> qty, tm time){
    Order order;
    order.date = time;

    unsigned int i =0;
    for (auto product : products) {
        order.products.emplace_back(product, qty.at(i++));
    }
    order.order_id = oid;
    assert(!(cm.findClient(client_id)==no_client));//to do throw
    order.client = cm.findClient(client_id);
    orders.emplace(order_id, order);
    order_id = (oid>order_id ? oid : order_id);
    order_id++;
    return true;
}

const Order& OrderManager::findOrder(const Order_ID order_id) const {
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


std::ofstream& OM::OrderManager::saveOrders(std::ofstream& out) const
{
    for (const auto& o : orders) {
        const Order& order = o.second;
        out << order.getID() << ',';
        out << order.getClient().getId() << ',';
        out << order.getDate()<<',';
        out << order.getProducts().size()<<','<<endl;
        for(const auto& product_info : order.getProducts()){
            out<<product_info.product<<','<<product_info.qty<<','<<endl;
        }
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
    return IteratorPTR<Order>(new OIterator{orders.begin()});
}
IteratorPTR<OM::Order> OrderManager::end(){
     return IteratorPTR<Order>(new OIterator{orders.end()});
}


const Order OrderManager::OIterator::operator*() const {
    return ptr->second;
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
