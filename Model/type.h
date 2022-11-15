#ifndef TYPE_H
#define TYPE_H
#include <string>
#include <QDebug>
namespace CM{
using std::string;
using CID = string;
class Client {
    friend class ClientManager;
public:
    Client(string id, string name, string phone_number , string address ) :
        id{ id }, name{ name }, phone_number{ phone_number }, address{ address } {}
    string getId() const { return id; }
    string getName() const { return name; }
    string getPhoneNumber() const { return phone_number; }
    string getAddress() const { return address; }
    Client()=default;
    ~Client(){}
    Client& operator= (const Client& rhs);
private:
    string id;
    string name;
    string phone_number;
    string address;
};
struct NoClient : public Client { NoClient() : Client{"","no_client","",""} {
        qDebug()<<"no_client address: "<<this;
    } };
extern const NoClient no_client;
bool operator== (const Client& c, const NoClient&);
}


namespace PM {
using std::string;
using PID = string;
class Product{
public:
    Product(string name, unsigned int price, unsigned int qty, std::tm date) :
        name{ name }, price{ price }, qty{ qty }, registered_date{date} {}
    Product(string id, string name, unsigned int price, unsigned int qty, std::tm date) :
        id{ id }, name{ name }, price{ price }, qty{ qty }, registered_date{ date }{}
    const string getId() const { return id; }
    const string getName() const { return name; }
    const unsigned int getPrice() const { return price; }
    const unsigned int getQty() const { return qty; }
    const std::tm getDate() const { return registered_date; }
    Product& operator= (const Product& rhs);
    Product() = default;
private:
    friend class ProductManager;
    string id;
    string name;
    unsigned int price;
    unsigned int qty;
    tm registered_date;
    bool decreaseQty(const unsigned int desc){
        return qty<desc ? false : qty-=desc , true;
    }
};

struct NoProduct : public Product { NoProduct() : Product{"","no_product",0,0,std::tm()} {
        qDebug()<<"no_product address: "<<this;
    } };
extern const NoProduct no_product;
bool operator== (const Product& p, const NoProduct&);

}


namespace OM {
using std::string;
using std::vector;
using Order_ID = unsigned int;
using namespace CM;
using namespace PM;
struct OrderedProduct{
    const Product product;
    unsigned int qty;
    OrderedProduct(const Product product, unsigned int qty):product{product},qty{qty}{}
};

struct Order {
public:
    using qty = unsigned int;
private:
    friend class OrderManager;
    Order_ID order_id;
    Client client;
    std::tm date;
    vector<std::pair<Product,qty>> products;
public:
    Order(Order_ID id, Client c, std::tm t, decltype(products) p) : order_id{id}, client{c}, date{t}, products{p} {}
    Order(){}
    const Order_ID getID() const {return order_id;}
    const Client& getClient() const {return client;}
    std::tm getDate() const {return date;}
    vector<OrderedProduct> getProducts() const;
};

struct NoOrder : public Order { NoOrder() : Order{0,no_client,std::tm(), {std::make_pair(no_product ,0)}}{}};
extern const NoOrder no_order;
inline bool operator== (const Order& o, const NoOrder&){ return (&o == &no_order ? true : false); }
inline bool operator!= (const Order& o, const NoOrder&){ return (&o != &no_order ? true : false); }

}
#endif // TYPE_H
