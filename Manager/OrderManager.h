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
namespace OM {
	using namespace CM;
	using namespace PM;
	using std::string;
	using std::vector;
	class OrderIterator;
	using Product_ID = unsigned int;
	using Client_ID = unsigned int;
	using Order_ID = unsigned int;
	
	struct No_Matching_Product { Product_ID pid; }; 
	struct No_Matching_Client { Client_ID cid; };	
	struct Already_In_Order { Order_ID oid; };		

	class OrderManager{
	public:
        OrderManager(ClientManager& cm, ProductManager& pm) :cm{ cm }, pm{ pm }{}
        struct ProductData{
            PM::PID id;
            unsigned int qty;
            ProductData(PM::PID pid, unsigned int qty): id{pid},qty{qty}{}
        };
        struct Order {
        private:
            friend class OrderManager;
			Order_ID order_id;				
			Client_ID client_id;			
			std::tm date;					
            vector<ProductData> products;
        public:
            const Order_ID getID() const {return order_id;}
            const Client_ID getCID() const {return client_id;}
            const std::tm getDate() const {return date;}
            const vector<ProductData> getProductData() const{return products;}
        };


        const ClientManager& cm;
        ProductManager& pm;
        const Order& findOrder(const Order_ID order_id) const;
		std::ofstream & saveOrders(std::ofstream&) const;												
		std::pair<std::ifstream&, vector<Order>> loadOrders(std::ifstream&);							
		const Product& getPurchasedProducts(const Product_ID pid) const;								
        void addOrder(const Order&);
        std::pair<const unsigned int, bool> addOrder(const Client_ID client_id, vector<ProductData>);
        OrderIterator getOrders() const;
        const unsigned int getSize() const {return orders.size();}
	private:
		unsigned int order_id = 0;								
		std::map<Order_ID, Order> orders;						
		std::multimap<Client_ID, Order*> orders_CID;			
		std::map<Product_ID, Product> purchased_products;		
		
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
