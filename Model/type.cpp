#include "type.h"
using namespace CM;
Client& Client::operator= (const Client& rhs){
    id=rhs.id;
    name=rhs.name;
    phone_number=rhs.phone_number;
    address=rhs.address;
    return *this;
}
