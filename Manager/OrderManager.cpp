
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
    std::cout<<"THIS IS SHARED "<<order.client.use_count()<<endl;
    auto inserted_order = orders.emplace(order_id, std::move(order));
	orders_CID.emplace(client_id, &inserted_order.first->second);

	return {order_id++, true};
}

void OrderManager::addOrder(const Order& order_to_add) {
//	Order order;

//	auto order_found = orders.find(order_to_add.order_id);
//	if (order_found != orders.end())
//		throw OM::Already_In_Order{ order_to_add.order_id};
//	order.order_id = order_to_add.order_id;

//	const Client& c = cm.findClient(order_to_add.client_id);
//	if (c == no_client)
//		throw OM::No_Matching_Client{ order_to_add.client_id};
//	order.client_id = order_to_add.client_id;

//    for (auto product : order_to_add.products) {
//        const Product& found = pm.findProduct(product.id);
//		if (found == no_product)
//            throw OM::No_Matching_Product{product.id};
		
//        auto itr = purchased_products.find(product.id);
//		if (itr == purchased_products.end()) {
//            auto inserted = purchased_products.emplace(product.id, Product{ found });
//            order.products.emplace_back(product.id, product.qty);
//		}
//		else {
//            order.products.emplace_back(product.id, product.qty);
//		}
//	}

//	order.date = order_to_add.date;
//	auto inserted_order = orders.emplace(order_to_add.order_id, std::move(order));
//	order_id = order_to_add.order_id;
//	order_id++;
//	orders_CID.emplace(order_to_add.client_id, &inserted_order.first->second);
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
	out << std::put_time(&p, "%A %c");
	return out;
}


static std::ofstream& operator<<(std::ofstream& out, const OrderManager::Order& o)
{
    out << o.getID() << ',';
    out << o.getClient() << ',';
    out << o.getDate();
    for (auto product : o.getProducts()) {
        out << ',' << product.product->getId();
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

std::pair<std::ifstream&, std::vector<OrderManager::Order>> OM::OrderManager::loadOrders(ifstream& in)
{
	std::vector<Order> order_vector;
	
	std::string str;
	while (getline(in, str)) {
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
        string time_string = tmp[2];
        Client_ID cid = (tmp[1]);
        Order_ID oid = stoul(tmp[0]);
		
		unsigned int idx = 3;
		while (idx<tmp.size()) {
            products_ids.emplace_back((tmp[idx++]));
		}

		std::tm time;
		istringstream ss{ time_string };
		ss >> std::get_time(&time, "%a %m/%d/%y %H:%M:%S");
//		Order od{ oid, cid, time, products_ids };
//		order_vector.emplace_back(od);
	}
	return  { in, (order_vector) };
}


const vector<OrderManager::OrderedProduct > OrderManager::Order::getProducts() const{
    vector<OrderedProduct> v;
    for(auto e : products){
        v.emplace_back(e.first.get(), e.second);
    }
    return v;
}
