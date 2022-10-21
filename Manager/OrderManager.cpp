
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
    if(products.empty()){
        return {0, false};
    }
    Order order;

	time_t base_time = time(nullptr);
	tm local_time;
	localtime_s(&local_time, &base_time);
	order.date = local_time;

    for (const auto &product : products) {
        Product& found = pm.findProduct(product.id);
		if (found == no_product)
            return { stoi(product.id), false };
        if (found.getQty()<product.qty)
            return { stoi(product.id), false };
    }
    int i=0;
    for (auto product : products) {
        order.products.emplace_back(pm.copyProduct(product.id), product.qty);
        assert(pm.buyProduct(product.id, product.qty));
        i++;
	}
	order.order_id = order_id;
    assert(!(cm.findClient(client_id)==no_client));
    order.client = cm.copyClient(client_id);
    auto inserted_order = orders.emplace(order_id, std::move(order));

	return {order_id++, true};
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
            vector<Product_ID> products_ids;
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
            Order_ID oid = stoul(tmp.at(0));

            Client_ID cid = tmp.at(1);
            cm.copyClient(cid);
            string time_string = tmp.at(2);
            vector<bill> bills;
            if((tmp.size()-3)%2!=0){
                throw;
            }
            for(auto b = tmp.begin()+3; b!=tmp.end(); ++b ){
                string pid {*b};
                ++b;
                unsigned int qty = stoul(*b);

            }



            unsigned int idx = 3;
            while (idx<tmp.size()) {
                products_ids.emplace_back((tmp[idx++]));
            }

            std::tm time;
            istringstream ss{ time_string };
            ss >> std::get_time(&time, "%D %T");

        }
        return  in;
    }
    catch(...){
        throw ERROR_WHILE_LOADING{line};
    }
}


const vector<OrderManager::OrderedProduct > OrderManager::Order::getProducts() const{
    vector<OrderedProduct> v;
    for(auto e : products){
        v.emplace_back(e.first.get(), e.second);
    }
    return v;
}
