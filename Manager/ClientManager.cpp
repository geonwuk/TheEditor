#include "ClientManager.h"

#include <fstream>
#include <string>


using namespace std;
using namespace CM;
unsigned int ClientManager::client_id = 0; 

bool ClientManager::addClient(const string id, const string name, const string phone_number, const string address) {
    bool success;
    tie(ignore, success) = clients.emplace(id, make_shared<Client>(id, name, phone_number, address));

	client_id++;
	return success;
}
void ClientManager::loadClient(const std::vector<CM::Client>& clients_to_add) {
    unsigned int line=0;
    for(const auto& c : clients_to_add){
        if(!addClient(c.getId(),c.getName(),c.getPhoneNumber(),c.getAddress())){
            throw ERROR_WHILE_LOADING{line};
        }
        ++line;
    }
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
const Client ClientManager::findClient(const CID id) const{
    auto it = clients.find(id);
    if (it == clients.end()) {
        return no_client;
    }
    else {
        return *it->second.get();
    }
}
Client ClientManager::copyClient(const CID id) const {
    auto it = clients.find(id);
    return Client{*it->second.get()};
}
ofstream& CM::operator<<(std::ofstream& out, const Client& c){
	out << c.getId() << ',' << c.getName() << ',' << c.getPhoneNumber() << ',' << c.getAddress();
	return out;
}
void ClientManager::checkSafeToLoad(const std::vector<CM::Client>& clients_to_load){
    unsigned int line=0;
    for(const auto& c : clients_to_load){
        if(clients.find(c.getId())!=clients.end()){     //만약 파일(DB, CSV)로부터 추가하려는 상품 ID가 이미 중복된 경우 로딩 불가입니다
            throw ERROR_WHILE_LOADING{line};            //어느 라인에서 로딩하다가 에러가 났는지 예외를 던집니다.
        }
        ++line;
    }
}
unsigned int ClientManager::getSize() const{
    return (unsigned int)clients.size();
}
bool CM::operator==(const Client& c, const NoClient&){
    const Client& nc { no_client };
	if (&c == &nc)
		return true;
	else
		return false;
}
