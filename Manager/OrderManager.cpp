
#include "OrderManager.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cassert>
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
        Product& found = pm.findProduct(product.id);
        if (found == no_product)
            return { stoi(product.id), false };
        if (found.getQty()<product.qty)
            return { stoi(product.id), false };
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

bool OrderManager::loadOrder(const Order_ID oid, const Client_ID client_id, vector<bill> products, tm time){
    Order order;
    order.date = time;

    for (const auto &product : products) {
        Product& found = pm.findProduct(product.id);
        if (found == no_product)
            return false;
    }
    for (auto product : products) {
        order.products.emplace_back(pm.findProduct(product.id), product.qty);
    }
    order.order_id = oid;
    assert(!(cm.findClient(client_id)==no_client));
    order.client = cm.findClient(client_id);
    auto inserted_order = orders.emplace(order_id, order);
    order_id = (oid>order_id ? oid : order_id);
    order_id++;
    return true;
}

OrderIterator OrderManager::getOrders() const
{
	return OrderIterator(orders.begin(), orders.end());
}

const OrderManager::Order& OrderManager::findOrder(const Order_ID order_id) const {
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


static std::ofstream& operator<<(std::ofstream& out, const OrderManager::Order& o)
{
    out << o.getID() << ',';
    out << o.getClient().getId() << ',';
    out << o.getDate();
    for (auto product_info : o.getProducts()) {
        out << ',' << product_info.product->getId()<<','<<product_info.qty;
	}
	return out;
}

std::ofstream& OM::OrderManager::saveOrders(std::ofstream& out) const
{
	for (const auto& p : orders) {
        out << p.second << endl;
	}
	return out;
}

std::ifstream& OM::OrderManager::loadOrders(ifstream& in, unsigned int lines)
{
    unsigned int line=0;
    try{
        std::string str;
        while (line++<lines && getline(in, str)) {
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
            Order_ID oid = static_cast<unsigned int>(stoi(tmp.at(0)));
            Client_ID cid = tmp.at(1);
            string time_string = tmp.at(2);
            std::tm time;
            istringstream ss{ time_string };
            ss >> std::get_time(&time, "%D %T");
            vector<bill> bills;
            if((tmp.size()-3)%2!=0){
                throw -1;
            }
            for(auto b = tmp.begin()+3; b!=tmp.end(); ++b ){
                PM::PID pid {*b};
                ++b;
                unsigned int qty = static_cast<unsigned int>(stoul(*b));
                bills.emplace_back(pid, qty);
            }
            auto result = loadOrder(oid, cid, bills, time);
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


const vector<OrderManager::OrderedProduct > OrderManager::Order::getProducts() const{
    vector<OrderedProduct> v;
    for(const auto& e : products){
        v.emplace_back(&e.first, e.second);
    }
    return v;
}
