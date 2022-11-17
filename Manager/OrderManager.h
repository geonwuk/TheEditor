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
        std::ifstream& loadOrders(std::ifstream&, unsigned int lines);
        std::pair<const Order_ID, bool> addOrder(const Client_ID client_id, vector<bill>) override;     //주문 추가를 위한 기능을 제공합니다
        const Order findOrder(const Order_ID order_id) const override;                                  //주문 조회 기능입니다
        const size_t getSize() const override {return orders.size();}                                   //주문 사이즈를 리턴합니다
        IteratorPTR<OM::Order> begin() override;    //range based for loop를 위한 begin()을 정의합니다
        IteratorPTR<OM::Order> end() override;      //range based for loop를 위한 end()을 정의합니다
	private:
        std::pair<const Order_ID, bool> addOrder(const Order_ID oid, const Client_ID client_id, vector<bill>, tm time);
        bool loadOrder(const Order_ID oid, const Client_ID client_id,  vector<Product> products, vector<unsigned int> qty, tm time) override;
        unsigned int order_id = 1;              //주문 ID는 숫자이며 1부터 시작해서 1씩 증가합니다
        std::map<Order_ID, Order> orders;       //주문 목록 컨테이너로 map을 씁니다
        const ClientModel& cm;                  //주문은 구매자 정보가 필요하므로 ClientModel을 선언합니다
        ProductModel& pm;                       //주문은 상품 정보가 필요하므로 ProductModel을 선언합니다
    public:
        struct OIterator : public Iterator<OM::Order> {                     //Iterator 패턴을 적용한 OrderManager Iterator를 정의합니다
        public:
            using Itr_type = decltype(orders)::const_iterator;              //std::map iterator를 그대로 씁니다
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
