#pragma once
#include <map>
#include <utility>
#include <string>
#include <iosfwd>
#include <vector>
#include <memory>
#include <iostream>
namespace CM {
	using std::map;
    using std::string;
    using CID = string;
	class Client {
        friend class ClientManager;
	public:

        Client(string name, string phone_number, string address) :
            name{ name }, phone_number{ phone_number }, address{ address } {}

        Client(string id, string name, string phone_number , string address ) :
            id{ id }, name{ name }, phone_number{ phone_number }, address{ address } {}
        const string getId() const { return id; }
		string getName() const { return name; }                  
		string getPhoneNumber() const { return phone_number; }   
        string getAddress() const { return address; }
        Client()=default;
        ~Client(){std::cout<<"CLIENT DESTRUCTOR ID: "<<id<<std::endl;}
    private:


        const string id;
		string name;           
		string phone_number;   
        string address;
        Client& operator= (const Client& rhs){
            name=rhs.name;
            phone_number=rhs.phone_number;
            address=rhs.address;
            return *this;
        }
	protected:

	};
    std::ofstream& operator<<(std::ofstream& out, const Client& c);
    struct NoClient : public Client { NoClient(){} };
    const NoClient no_client;
	bool operator== (const Client& c, const NoClient&);   

	class ClientManager{
	public:
        class const_iterator;
        bool addClient(const CID id, const string name, const string phone_number = "NONE", const string address = "NONE");
        bool modifyClient(const CID id, const Client client);
        bool eraseClient(const CID id);
        const Client& findClient(const CID id) const;
        std::shared_ptr<Client> copyClient(const CID) const noexcept;
		std::ofstream& saveClients(std::ofstream& out) const;								                 
		std::pair<std::ifstream&, std::vector<Client>> loadClients(std::ifstream& in) const ;                
        const_iterator getCleints() const;
//		const unsigned int getMaxIndex() const;
        const unsigned int getSize() const;
	private:
		static unsigned int client_id;
        map< CID, std::shared_ptr<Client> > clients;
    public:
        class const_iterator{
            using Itr_type = decltype(clients)::const_iterator;
            struct Itr {
                Itr_type ptr;
                Itr(Itr_type p) :ptr{ p } {}
                const Client& operator*() const {
                    return *ptr->second.get();
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




