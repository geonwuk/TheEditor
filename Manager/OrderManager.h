#pragma once
#include <ctime>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <utility>
#include "ProductManager.h"
#include "ClientManager.h"
#include <functional>
namespace OM {
	using namespace CM;
	using namespace PM;
	using std::string;
	using std::vector;
	class OrderIterator;
    using Product_ID = PM::PID;
    using Client_ID = CM::CID;
	using Order_ID = unsigned int;
	
	struct No_Matching_Product { Product_ID pid; }; 
	struct No_Matching_Client { Client_ID cid; };	
	struct Already_In_Order { Order_ID oid; };		

	class OrderManager{
	public:
        OrderManager(ClientManager& cm, ProductManager& pm) :cm{ cm }, pm{ pm }{}
        struct OrderedProduct{
            Product* product;
            unsigned int qty;
            OrderedProduct(Product* product, unsigned int qty):product{product},qty{qty}{}
        };

        struct Order {
        private:
            friend class OrderManager;
			Order_ID order_id;				
            std::shared_ptr<Client> client;
            std::tm date;
            using qty = unsigned int;
            vector<std::pair<std::shared_ptr<Product>,qty>> products;
        public:
            const Order_ID getID() const {return order_id;}
            const Client& getClient() const {return *client.get();}
            const std::tm getDate() const {return date;}
            const vector<OrderedProduct> getProducts() const;
        };


        const ClientManager& cm;
        ProductManager& pm;
        const Order& findOrder(const Order_ID order_id) const;
		std::ofstream & saveOrders(std::ofstream&) const;												
		std::pair<std::ifstream&, vector<Order>> loadOrders(std::ifstream&);															
        void addOrder(const Order&);
        struct bill{
            PM::PID id;
            unsigned int qty;
            bill(PM::PID id,unsigned int qty):id{id},qty{qty}{}
        };
        std::pair<const Order_ID, bool> addOrder(const Client_ID client_id, vector<bill>);
        OrderIterator getOrders() const;
        const size_t getSize() const {return orders.size();}
	private:
		unsigned int order_id = 0;								
		std::map<Order_ID, Order> orders;						
		std::multimap<Client_ID, Order*> orders_CID;				
		
	public:
		using OM_itr = decltype(OrderManager::orders)::const_iterator;
	};
	struct NoOrder : public OrderManager::Order { };
    const NoOrder no_order{};
    inline bool operator== (const OrderManager::Order& o, const NoOrder&){ return (&o == &no_order ? true : false); }
    inline bool operator!= (const OrderManager::Order& o, const NoOrder&){ return (&o != &no_order ? true : false); }

	using Itr_type = OrderManager::OM_itr;

	class OrderIterator { 
	private:
		struct Itr {
			Itr(Itr_type p) :ptr{ p } {}
			const OrderManager::Order& operator*() const {
				return ptr->second;
			}
			Itr_type operator++() {
				return ++ptr;
			}
			bool operator!=(Itr b) {
				auto re = (ptr) != (b.ptr);
				return re;
			}
        private:
            Itr_type ptr;
		};
	public:
		OrderIterator(Itr_type b, Itr_type e) : st{ b }, ed{ e } {}
		Itr begin() {return (st);}  
        Itr end() {return (ed);}
    private:
        Itr st, ed;
	};
}
