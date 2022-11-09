#pragma once
#include <map>
#include <utility>
#include <string>
#include <iosfwd>
#include <vector>
#include <iostream>
#include "Model/type.h"
#include "Model/model.h"
namespace CM {
	using std::map;
    using std::string;
    struct ERROR_WHILE_LOADING {unsigned int line;};

    std::ofstream& operator<<(std::ofstream& out, const Client& c);
    struct NoClient : public Client { NoClient(){} };
    const NoClient no_client;
	bool operator== (const Client& c, const NoClient&);   

    class ClientManager : public ClientModel{
	public:
        ClientManager():iterator{clients} {}
        class const_iterator;
        bool addClient(const CID id, const string name, const string phone_number = "NONE", const string address = "NONE") override;
        bool modifyClient(const CID id, const Client client) override;
        bool eraseClient(const CID id)override;
        const Client& findClient(const CID id) const override;
        std::shared_ptr<Client> copyClient(const CID) const noexcept;
		std::ofstream& saveClients(std::ofstream& out) const;								                 
        std::ifstream& loadClients(std::ifstream& in, const unsigned int lines);

        unsigned int getSize() const override;
	private:
		static unsigned int client_id;
        map< CID, std::shared_ptr<Client> > clients;
    public:

        class const_iterator : public Iterator<Client> {
        public:
            friend class ClientManager;
            using Itr_type = decltype(clients)::const_iterator;

            struct Itr : public IteratorElem<CM::Client> {
                friend class ClientManager;
                Itr(Itr_type p) :ptr{ p } {}

                const Client operator*() const override {
                    return *ptr->second.get();
                }
                void operator++() override {
                    ++ptr;
                }
                IteratorElem& operator= (IteratorElem& rhs) override{
                    ptr = static_cast<Itr&>(rhs).ptr;
                    return *this;
                }
                bool operator!=(IteratorElem& b) override {
                    return ptr != static_cast<Itr&>(b).ptr ? true : false;
                }
                bool operator==(IteratorElem& b) override {
                    return ptr == static_cast<Itr&>(b).ptr ? true : false;
                }
                std::unique_ptr<CM::Client> operator->() override{
                    return std::unique_ptr<CM::Client>();
                }
            private:
                Itr_type ptr;
            };

            Itr st;
            Itr ed;
            const decltype(clients)& container;
        public:
            const_iterator(const decltype(clients)& c): st{c.begin()}, ed{c.end()}, container{c} {}
            const Itr* Begin() override {
                st.ptr=container.begin();
                ed.ptr=container.end();
                return &st;
            }
            const Itr* End() override {
                return &ed;
            }
        };
    public:
        std::unique_ptr<IteratorElem<CM::Client>> itreator() override{
            return std::unique_ptr<IteratorElem<CM::Client>> {static_cast<IteratorElem<CM::Client>*>(new const_iterator::Itr{clients.begin()})};
        }
        Iterator<CM::Client>* getClients() override{
            return &iterator;
        }

    public:
        const_iterator iterator;
	};

}




