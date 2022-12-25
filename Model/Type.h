#ifndef TYPE_H
#define TYPE_H

#include <utility>
#include <string>

#include <vector>

namespace CM{
using CID = std::string;
class Client {
    friend class ClientManager;
public:
    Client(std::string id, std::string name, std::string phone_number , std::string address ) :
        id{ id }, name{ name }, phone_number{ phone_number }, address{ address } {}
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getPhoneNumber() const { return phone_number; }
    std::string getAddress() const { return address; }
    Client()=default;
    ~Client(){}
    Client& operator= (const Client& rhs);
private:
    std::string id;
    std::string name;
    std::string phone_number;
    std::string address;
};
struct NoClient : public Client { NoClient() : Client{"","no_client","",""} {} };
extern const NoClient no_client;
bool operator== (const Client& c, const NoClient&);
} //namespace CM

namespace PM {
using PID = std::string;
class Product{
public:
    Product(std::string name, unsigned int price, unsigned int qty, std::tm date) :
        name{ name }, price{ price }, qty{ qty }, registered_date{date} {}
    Product(std::string id, std::string name, unsigned int price, unsigned int qty, std::tm date) :
        id{ id }, name{ name }, price{ price }, qty{ qty }, registered_date{ date }{}
    const std::string getId() const { return id; }
    const std::string getName() const { return name; }
    const unsigned int getPrice() const { return price; }
    const unsigned int getQty() const { return qty; }
    const std::tm getDate() const { return registered_date; }
    Product& operator= (const Product& rhs);
    Product() = default;
private:
    friend class ProductManager;
    std::string id;
    std::string name;
    unsigned int price;
    unsigned int qty;
    tm registered_date;
    bool decreaseQty(const unsigned int desc){
        return qty<desc ? false : qty-=desc , true;
    }
};
struct NoProduct : public Product { NoProduct() : Product{"","no_product",0,0,std::tm()} {} };
extern const NoProduct no_product;
bool operator== (const Product& p, const NoProduct&);
} //namespace PM


namespace OM {
using Order_ID = unsigned int;          //주문의 경우 1부터 시작하는 정수 ID를 갖습니다
struct OrderedProduct{                  //Order 클래스에서 주문한 상품
    const PM::Product product;
    unsigned int qty;
    OrderedProduct(const PM::Product product, unsigned int qty):product{product},qty{qty}{}
};
struct Order {
public:
    using qty = unsigned int;
private:
    friend class OrderManager;
    Order_ID order_id;                          //주문번호 (1부터 시작)
    CM::Client client;                              //구매자
    std::tm date;                               //구매날짜
<<<<<<< HEAD:Model/type.h
    std::vector<std::pair<Product,qty>> products;    //구매한 물품들
=======
    std::vector<std::pair<PM::Product,qty>> products;    //구매한 물품들
>>>>>>> refactoring:Model/Type.h
public:
    Order(Order_ID id, CM::Client c, std::tm t, decltype(products) p) : order_id{id}, client{c}, date{t}, products{p} {}
    Order(){}
    const Order_ID getID() const {return order_id;}
    const CM::Client& getClient() const {return client;}
    std::tm getDate() const {return date;}
    std::vector<OrderedProduct> getProducts() const;
};
struct NoOrder : public Order { NoOrder() : Order{0, CM::no_client,std::tm(), {std::make_pair(PM::no_product ,0)}}{}};
extern const NoOrder no_order;
inline bool operator== (const Order& o, const NoOrder&){ return (&o == &no_order ? true : false); }
inline bool operator!= (const Order& o, const NoOrder&){ return (&o != &no_order ? true : false); }
} //namespace OM

#endif // TYPE_H
