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
    out << p.getDate()<<',';
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
    bool success;
    tie(ignore, success) = products.emplace(ID, Product(ID, name, price, qty, local_time));
    product_id++;
    return success;
}

bool ProductManager::addProduct(const string id, const string name, const unsigned int price, const unsigned int qty, tm time)
{
    bool success;
    tie(ignore, success) = products.emplace(id, Product(id, name, price, qty, time));
    product_id++;
    return success;
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

bool ProductManager::buyProduct(const PID id, const unsigned int qty) {
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


ofstream& PM::ProductManager::saveProducts(ofstream& out) const{
	for (const auto& p : products) {
        out << p.second << endl;
	}
	return out;
}

std::ifstream& PM::ProductManager::loadProducts(ifstream& in, const unsigned int lines){
    unsigned int line=0;
    try{
        string str;
        while (line++<lines && getline(in, str)){
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

            string time_string = tmp.at(4);
            unsigned int qty = stoul(tmp.at(3));
            unsigned int price = stoul(tmp.at(2));
            string name = tmp.at(1);
            string id = tmp.at(0);

            tm time;
            istringstream ss{ time_string };
            ss >> std::get_time(&time, "%D %T");

            if(!addProduct(id,name,price,qty,time)){
                throw -1;
            }

        }
        return  in;
    }
    catch(...){
        throw ERROR_WHILE_LOADING{line};
    }
}

const unsigned int ProductManager::getSize() const{
    return products.size();
}
