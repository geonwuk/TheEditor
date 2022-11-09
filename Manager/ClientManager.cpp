#include "ClientManager.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>
#include <functional>

using namespace std;
using namespace CM;
unsigned int ClientManager::client_id = 0; 


bool ClientManager::addClient(const string id, const string name, const string phone_number, const string address) {
    bool success;
    tie(ignore, success) = clients.emplace(id, make_shared<Client>(id, name, phone_number, address));

	client_id++;
	return success;
}

bool ClientManager::modifyClient(const CID id, const Client new_client){
    auto it = clients.find(id);
    if (it == clients.end()) return false;
    Client& the_client = *it->second.get();
    the_client = new_client;
    return true;
}

bool ClientManager::eraseClient(const CID id){
	using int_type = decltype(clients)::size_type;
	int_type success = clients.erase(id);
    //cout<<"use count of "<< clients[0].use_count();
	if (success == 1)
		return true;
	else
		return false;
}

const Client& ClientManager::findClient(const CID id) const{
    auto it = clients.find(id);
    if (it == clients.end()) {
        return no_client;
    }
    else {
        return *it->second.get();
    }
}

std::shared_ptr<Client> ClientManager::copyClient(const CID id) const noexcept{
    auto it = clients.find(id);
    return it->second;
}

ofstream& CM::operator<<(std::ofstream& out, const Client& c){
	out << c.getId() << ',' << c.getName() << ',' << c.getPhoneNumber() << ',' << c.getAddress();
	return out;
}



ofstream& ClientManager::saveClients(ofstream& out) const{
	for (const auto& c : clients) {
        out << *c.second.get()<<',' << endl;
	}
	return out;
}

std::ifstream& ClientManager::loadClients(std::ifstream& in, const unsigned int lines) {
    unsigned int line =0;
    try{
        string str;
        while (line++<lines && getline(in, str)) {
            vector<string> tmp;
            auto beg = str.find_first_not_of(',');
            while (beg != string::npos) {
                auto endIdx = str.find_first_of(',', beg);
                if (endIdx == string::npos) {
                    endIdx = str.length();
                }
                tmp.emplace_back(str.substr(beg, endIdx - beg));
                beg = str.find_first_not_of(',', endIdx);
            }
            string address = tmp.at(3);
            string phone_number = tmp.at(2);
            string name = tmp.at(1);
            string id = tmp.at(0);
            addClient(id, name, phone_number, address);
        }
        return in;
    }
    catch(...){
        throw ERROR_WHILE_LOADING{line};
    }

}

unsigned int ClientManager::getSize() const{
    return clients.size();
}

bool CM::operator==(const Client& c, const NoClient&){
	const Client& nc { no_client };
	if (&c == &nc)
		return true;
	else
		return false;
}
