
#include "OrderManager.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cassert>
using namespace std;
using namespace OM;
using namespace PM;
const Product& OM::OrderManager::getPurchasedProducts(const Product_ID pid) const{
	auto it = purchased_products.find(pid);	
	if (it == purchased_products.end()) {
		return no_product;
	}
	else {
		return it->second;
	}
}

std::pair<const unsigned int, bool> OrderManager::addOrder(const Client_ID client_id, vector<ProductData> products){
    if(products.empty()){
        return {0, false};
    }
    Order order;

	time_t base_time = time(nullptr);
	tm local_time;
	localtime_s(&local_time, &base_time);
	order.date = local_time;

    vector<Product*> found_products;
    for (auto product : products) {
        Product& found = pm.findProduct(product.id);
		if (found == no_product)
            return { product.id, false };
        if (found.getQty()<product.qty)
            return { product.id, false };
        found_products.emplace_back(&found);
    }
    int i=0;
    for (auto product : products) {
        auto itr = purchased_products.find(product.id);
		if (itr == purchased_products.end()) {			
            purchased_products.emplace(product.id, Product{ *found_products[i] });
            order.products.emplace_back(product.id, product.qty);
		}
		else {
            order.products.emplace_back(product.id, product.qty);
		}
        assert(found_products[i]->decreaseQty(product.qty));
        i++;
	}
	order.order_id = order_id;
	order.client_id = client_id;
	auto inserted_order = orders.emplace(order_id, std::move(order));
	orders_CID.emplace(client_id, &inserted_order.first->second);

	return {order_id++, true};
}

void OrderManager::addOrder(const Order& order_to_add) {
	Order order;

	auto order_found = orders.find(order_to_add.order_id);
	if (order_found != orders.end())
		throw OM::Already_In_Order{ order_to_add.order_id};	
	order.order_id = order_to_add.order_id;

	const Client& c = cm.findClient(order_to_add.client_id);
	if (c == no_client)
		throw OM::No_Matching_Client{ order_to_add.client_id};
	order.client_id = order_to_add.client_id;

    for (auto product : order_to_add.products) {
        const Product& found = pm.findProduct(product.id);
		if (found == no_product)
            throw OM::No_Matching_Product{product.id};
		
        auto itr = purchased_products.find(product.id);
		if (itr == purchased_products.end()) {			
            auto inserted = purchased_products.emplace(product.id, Product{ found });
            order.products.emplace_back(product.id, product.qty);
		}
		else {
            order.products.emplace_back(product.id, product.qty);
		}
	}

	order.date = order_to_add.date;													
	auto inserted_order = orders.emplace(order_to_add.order_id, std::move(order));	
	order_id = order_to_add.order_id;
	order_id++;
	orders_CID.emplace(order_to_add.client_id, &inserted_order.first->second);
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
    out << o.getID() << ',' << o.getCID() << ',';
    out << o.getDate();
    for (auto product : o.getProductData()) {
        out << ',' << product.id;
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
        Client_ID cid = stoul(tmp[1]);
        Order_ID oid = stoul(tmp[0]);
		
		unsigned int idx = 3;
		while (idx<tmp.size()) {
			products_ids.emplace_back(stoul(tmp[idx++]));
		}

		std::tm time;
		istringstream ss{ time_string };
		ss >> std::get_time(&time, "%a %m/%d/%y %H:%M:%S");
//		Order od{ oid, cid, time, products_ids };
//		order_vector.emplace_back(od);
	}
	return  { in, (order_vector) };
}
