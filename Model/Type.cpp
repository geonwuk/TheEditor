#include "Type.h"
using namespace CM;
using namespace std;
extern const NoClient CM::no_client{};
Client& Client::operator= (const Client& rhs){
    id=rhs.id;
    name=rhs.name;
    phone_number=rhs.phone_number;
    address=rhs.address;
    return *this;
}

using namespace PM;
extern const NoProduct PM::no_product{};
Product& Product::operator= (const Product& rhs){
    name=rhs.name;
    price=rhs.price;
    qty=rhs.qty;
    registered_date=rhs.registered_date;
    return *this;
}

using namespace OM;
extern const NoOrder OM::no_order{};

std::vector<OrderedProduct> Order::getProducts() const{
    std::vector<OrderedProduct> v;
    for(const auto& e : products){
        v.emplace_back(e.first, e.second);
    }
    return v;
}
