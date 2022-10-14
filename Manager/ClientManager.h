#pragma once
#include <map>
#include <utility>
#include <string>
#include <iosfwd>
#include <vector>

namespace CM {
	using std::map;
    using std::string;
    using CID = unsigned int;
	class Client {
	public:
		Client(unsigned int id, string name, string phone_number = "NONE", string address = "NONE") :
			id{ id }, name{ name }, phone_number{ phone_number }, address{ address } {}
        const CID getId() const { return id; }
		string getName() const { return name; }                  
		string getPhoneNumber() const { return phone_number; }   
        string getAddress() const { return address; }
	private:
        CID id;
		string name;           
		string phone_number;   
		string address;        
	protected:
        Client()=default;
	};

    struct NoClient : public Client { NoClient(){} };
    const NoClient no_client;
	bool operator== (const Client& c, const NoClient&);   


	class ClientManager{
	public:
        class const_iterator;
		bool addClient(const string name, const string phone_number = "NONE", const string address = "NONE");
        bool eraseClient(const CID id);
        const Client& findClient(const CID id) const;
		std::ofstream& saveClients(std::ofstream& out) const;								                 
		std::pair<std::ifstream&, std::vector<Client>> loadClients(std::ifstream& in) const ;                
        const_iterator getCleints() const;
		const unsigned int getMaxIndex() const;
        const unsigned int getSize() const;
	private:
		static unsigned int client_id;
		map< unsigned int, Client > clients;
    public:
        class const_iterator{
            using Itr_type = decltype(clients)::const_iterator;
            struct Itr {
                Itr_type ptr;
                Itr(Itr_type p) :ptr{ p } {}
                const Client& operator*() const {
                    return ptr->second;
                }
                Itr_type operator++() {
                    return ++ptr;
                }
                bool operator!=(Itr b) {
                    return ptr != b.ptr ? true : false;
                }
            };
            Itr st, ed;
        public:
            const_iterator(const decltype(clients)& c): st{c.begin()}, ed{c.end()} {}
            Itr begin() {return st;}
            Itr end() {return ed;}
        };

	};

}




