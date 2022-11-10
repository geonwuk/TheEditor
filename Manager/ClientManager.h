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

    class ClientManager : public ClientModel{
	public:
        class const_iterator;
        bool addClient(const CID id, const string name, const string phone_number = "NONE", const string address = "NONE") override;
        bool modifyClient(const CID id, const Client client) override;
        bool eraseClient(const CID id)override;
        const Client findClient(const CID id) const override;
        Client copyClient(const CID) const override;
		std::ofstream& saveClients(std::ofstream& out) const;								                 
        std::ifstream& loadClients(std::ifstream& in, const unsigned int lines);

        unsigned int getSize() const override;
	private:
		static unsigned int client_id;
        map< CID, std::shared_ptr<Client> > clients;
    private:
        class CIterator : public Iterator<Client> {
        public:
            friend class ClientManager;
            using Itr_type = decltype(clients)::const_iterator;
            CIterator(Itr_type p) : ptr{ p } {}
            ~CIterator() {}
            const Client operator*() const override {
                return *ptr->second.get();
            }
            void operator++() override {
                ++ptr;
            }
//            CIterator& operator= (Iterator& rhs) override{
//                ptr = static_cast<CIterator&>(rhs).ptr;
//                return *this;
//            }
            bool operator!=(Iterator& b) override {
                return (ptr != static_cast<CIterator&>(b).ptr ? true : false);
            }
            bool operator==(Iterator& b) override {
                return ptr == static_cast<CIterator&>(b).ptr ? true : false;
            }
//            Client operator->() override{
//                return *ptr->second.get();
//            }
        private:
            Itr_type ptr;
        };
    public:
        IteratorPTR<CM::Client> begin() override {
            return IteratorPTR<CM::Client>(new CIterator{clients.begin()});
        }
        IteratorPTR<CM::Client> end() override {
            return IteratorPTR<CM::Client>(new CIterator{clients.end()});
        }
    };
}


