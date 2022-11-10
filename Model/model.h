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
    IteratorPTR(ITR* itr) : std::unique_ptr<ITR>{itr} {}
    const T operator*() const{return **std::unique_ptr<ITR>::get();}
    void operator++(){ ++*std::unique_ptr<ITR>::get();}
    bool operator!=(const IteratorPTR& rhs){return *std::unique_ptr<ITR>::get()!=*rhs.get();}
    bool operator==(const IteratorPTR& rhs){return *std::unique_ptr<ITR>::get()==*rhs.get();}
//    T* operator->() const {return std::unique_ptr<ITR>::get()->operator->();}
};
class ClientModel{
public:
    virtual unsigned int getSize() const = 0;
    virtual const CM::Client findClient(const CM::CID) const =0;
    virtual bool addClient(std::string, std::string, std::string, std::string) = 0;
    virtual bool modifyClient(const CM::CID, const CM::Client) = 0;
    virtual bool eraseClient(const CM::CID) = 0;
    virtual CM::Client copyClient(const CM::CID) const =0;
    virtual IteratorPTR<CM::Client> begin()=0;
    virtual IteratorPTR<CM::Client> end()=0;
};

#endif // MODEL_H
