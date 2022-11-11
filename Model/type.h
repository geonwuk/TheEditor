#ifndef TYPE_H
#define TYPE_H
#include <string>
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
struct NoClient : public Client { NoClient() : Client{"","no_client","",""} {} };
const NoClient no_client;
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
    Product& operator= (const Product& rhs){
        name=rhs.name;
        price=rhs.price;
        qty=rhs.qty;
        registered_date=rhs.registered_date;
        return *this;
    }
protected:
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

struct NoProduct : public Product { NoProduct() : Product{"","no_product",0,0,std::tm()} {} };
const NoProduct no_product;
bool operator== (const Product& p, const NoProduct&);

}

#endif // TYPE_H
