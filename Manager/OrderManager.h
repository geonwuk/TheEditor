#pragma once
#include <ctime>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <utility>
#include <functional>
#include "Model/model.h"
namespace OM {
	using namespace CM;
	using namespace PM;
	using std::string;
	using std::vector;
	class OrderIterator;
    using Product_ID = PM::PID;
    using Client_ID = CM::CID;

    struct ERROR_WHILE_LOADING { unsigned int line; };

    class OrderManager : public OrderModel{
	public:
        OrderManager(ClientModel& cm, ProductModel& pm) :cm{ cm }, pm{ pm }{}
        ;									
        std::ifstream& loadOrders(std::ifstream&, unsigned int lines);
        std::pair<const Order_ID, bool> addOrder(const Client_ID client_id, vector<bill>) override;
        const Order findOrder(const Order_ID order_id) const override;
        const size_t getSize() const override {return orders.size();}
        IteratorPTR<OM::Order> begin() override;
        IteratorPTR<OM::Order> end() override;

	private:
        std::pair<const Order_ID, bool> addOrder(const Order_ID oid, const Client_ID client_id, vector<bill>, tm time);
        bool loadOrder(const Order_ID oid, const Client_ID client_id,  vector<Product> products, vector<unsigned int> qty, tm time);
		unsigned int order_id = 0;								
        std::map<Order_ID, Order> orders;
        const ClientModel& cm;
        ProductModel& pm;

    public:
        struct OIterator : public Iterator<OM::Order> {
        public:
            using Itr_type = decltype(orders)::const_iterator;
            OIterator(Itr_type o) : ptr{ o } {}
            const Order operator*() const override;
            virtual void operator++() override;
            virtual bool operator!=(Iterator& ) override;
            virtual bool operator==(Iterator& ) override;
        private:
            Itr_type ptr;
        };
	};



    std::ofstream& operator<<(std::ofstream& out, const Order& );
}
