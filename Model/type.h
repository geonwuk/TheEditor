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
struct NoClient : public Client { NoClient(){} };
const NoClient no_client;
bool operator== (const Client& c, const NoClient&);

}


#endif // TYPE_H
