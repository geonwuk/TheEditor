#ifndef MODEL_H
#define MODEL_H
#include "Model/type.h"
#include <memory>
#include <utility>
#include <QDebug>
template<typename T>
class Iterator{
public:
    virtual const T operator*() const =0;
    //virtual Iterator& operator= (Iterator&)=0;
    virtual void operator++() =0;
    virtual bool operator!=(Iterator& )=0;
    virtual bool operator==(Iterator& )=0;
    virtual ~Iterator(){};
};
template<typename T, typename ITR=Iterator<T>>
struct IteratorPTR : private std::unique_ptr<ITR>{
    explicit IteratorPTR( ITR* itr) : std::unique_ptr<ITR>{itr} {}
    const T operator*() const{return **std::unique_ptr<ITR>::get();}
    void operator++(){ ++*std::unique_ptr<ITR>::get();}
    bool operator!=(const IteratorPTR& rhs){return *std::unique_ptr<ITR>::get()!=*rhs.get();}
    bool operator==(const IteratorPTR& rhs){return *std::unique_ptr<ITR>::get()==*rhs.get();}
//    T* operator->() const {return std::unique_ptr<ITR>::get()->operator->();}
};
class ClientModel{
public:
    virtual ~ClientModel(){}
    virtual unsigned int getSize() const = 0;
    virtual const CM::Client findClient(const CM::CID) const =0;
    virtual bool addClient(std::string, std::string, std::string, std::string) = 0;
    virtual bool modifyClient(const CM::CID, const CM::Client) = 0;
    virtual bool eraseClient(const CM::CID) = 0;
    virtual CM::Client copyClient(const CM::CID) const = 0;
    virtual IteratorPTR<CM::Client> begin() const = 0;
    virtual IteratorPTR<CM::Client> end() const = 0;
};

class ProductModel{
public:
    virtual ~ProductModel(){}
    virtual bool addProduct(const std::string name, const unsigned int price, const unsigned int qty)=0;
    virtual bool loadProduct(const std::string id, const std::string name, const unsigned int price, const unsigned int qty, std::tm) noexcept(false) =0 ;
    virtual bool modifyProduct(const PM::PID id, const PM::Product new_product)=0;
    virtual bool eraseProduct(const PM::PID id)=0;
    virtual const PM::Product findProduct(const PM::PID id) const=0;
    virtual bool buyProduct(const PM::PID id, const unsigned int qty)=0;
    virtual const unsigned int getSize() const=0;
    virtual IteratorPTR<PM::Product> begin()=0;
    virtual IteratorPTR<PM::Product> end()=0;
};


class OrderModel{
public:
    struct bill{
        PM::PID id;
        unsigned int qty;
        bill(PM::PID id,unsigned int qty):id{id},qty{qty}{}
    };
    virtual ~OrderModel(){}
    virtual std::pair<const OM::Order_ID, bool> addOrder(const CM::CID client_id, std::vector<bill>)=0;
    virtual bool loadOrder(const OM::Order_ID oid, const CM::CID client_id, std::vector<PM::Product> products, std::vector<unsigned int> qty, tm time) noexcept(false) =0;
    virtual const OM::Order findOrder(const OM::Order_ID order_id) const=0;
    virtual const size_t getSize() const =0;
    virtual IteratorPTR<OM::Order> begin()=0;
    virtual IteratorPTR<OM::Order> end()=0;

};

#endif // MODEL_H
