#include "ClientManager.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
using namespace CM;
unsigned int ClientManager::client_id = 0; 

bool ClientManager::addClient(const string name, const string phone_number, const string address) {
	bool success;
    tie(ignore, success) = clients.emplace( client_id, Client{ client_id, name, phone_number, address } );
	client_id++;
	return success;
}

bool ClientManager::eraseClient(const CID id){
	using int_type = decltype(clients)::size_type;
	int_type success = clients.erase(id);
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
		return it->second;
	}
}

ofstream& operator<<(std::ofstream& out, const Client& c){
	out << c.getId() << ',' << c.getName() << ',' << c.getPhoneNumber() << ',' << c.getAddress();
	return out;
}

ClientManager::const_iterator ClientManager::getCleints() const{
    return clients;
}


ofstream& ClientManager::saveClients(ofstream& out) const{
	for (const auto& c : clients) {
		out << c.second << endl;
	}
	return out;
}

std::pair<std::ifstream&, std::vector<Client>> ClientManager::loadClients(std::ifstream& in) const {
	vector<Client> client_vector;
	std::string str;
	while (std::getline(in, str)) {
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
		string address = tmp[3];
		string phone_number = tmp[2];
		string name = tmp[1];
		unsigned int id = stoul(tmp[0]);
		client_vector.emplace_back(id, name, phone_number, address);
	}
	return {in, move(client_vector)}; 
}

const unsigned int ClientManager::getMaxIndex() const{
    return clients.empty() ? 0 : (--clients.end())->first;
}

const unsigned int ClientManager::getSize() const{
    return clients.size();
}

bool CM::operator==(const Client& c, const NoClient&){
	const Client& nc { no_client };
	if (&c == &nc)
		return true;
	else
		return false;
}
