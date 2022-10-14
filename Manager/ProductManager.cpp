#include "ProductManager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;
using namespace PM;

unsigned int ProductManager::product_id=0;
NoProduct no_product {};
std::ofstream& operator<< (std::ofstream& out, tm p) {
	out << std::put_time(&p, "%A %c");
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
	bool success;
	tie(ignore, success) = products.emplace(product_id, Product{ product_id, name, price, qty, local_time });
	product_id++;
	return success;
}

bool ProductManager::eraseProduct(const unsigned int id){
	using int_type = decltype(products)::size_type;
	int_type success = products.erase(id);
	if (success == 1)
		return true;
	else
		return false;
}

Product &ProductManager::findProduct(const unsigned int id) {
    auto it = products.find(static_cast<unsigned int>(id));
	if (it == products.end()) {
		return no_product;
	}
	else {
		return it->second;
	}
}

const Product& ProductManager::findProduct(const unsigned int id) const{
    auto it = products.find(static_cast<unsigned int>(id));
    if (it == products.end()) {
        return no_product;
    }
    else {
        return it->second;
    }
}

ProductManager::const_iterator ProductManager::getProducts() const{
	return products;
}

const Product& PM::ProductManager::getProduct(const unsigned int id) const{
	auto p = products.find(id);
	if (p == products.end()) {
		return no_product;
	}
	else {
		return p->second;
	}
}

ofstream& PM::ProductManager::saveProducts(ofstream& out) const{
	for (const auto& p : products) {
		out << p.second << endl;
	}
	return out;
}

std::pair<std::ifstream&, std::vector<Product>> PM::ProductManager::loadProducts(ifstream& in){
	vector<Product> product_vector;
	std::string str;
	while (getline(in, str)){
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

		string time_string = tmp[4];
		unsigned int qty = stoul(tmp[3]);
		unsigned int price = stoul(tmp[2]);
		string name = tmp[1];
		unsigned int id = stoul(tmp[0]);

		tm time;
		istringstream ss{ time_string };
		ss >> std::get_time(&time, "%a %m/%d/%y %H:%M:%S");	
		product_vector.emplace_back(id, name, price, qty, time);
	}
	return  { in, move(product_vector) };
}

const unsigned int PM::ProductManager::getMaxIndex() const {
    return products.empty() ? 0 : (--products.end())->first;
}

const unsigned int ProductManager::getSize() const{
    return products.size();
}
