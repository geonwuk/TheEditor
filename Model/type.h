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
    const string id;
    string name;
    string phone_number;
    string address;
};
}


#endif // TYPE_H
