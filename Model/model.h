#ifndef MODEL_H
#define MODEL_H
#include "Model/type.h"
#include <memory>
template<typename T>
class IteratorElem{
public:
    virtual const T operator*() const =0;
    virtual IteratorElem& operator= (IteratorElem&)=0;
    virtual void operator++() =0;
    virtual bool operator!=(IteratorElem& )=0;
    virtual bool operator==(IteratorElem& )=0;
    virtual std::unique_ptr<T> operator->()=0;

};
template<typename T>
class Iterator{
public:
    virtual const IteratorElem<T>* Begin()=0;
    virtual const IteratorElem<T>* End()=0;
};

using std::string;
class ClientModel
{
public:
    virtual unsigned int getSize() const = 0;
    virtual const CM::Client& findClient(const CM::CID) const =0;
    virtual bool addClient(string, string, string, string) = 0;
    virtual bool modifyClient(const CM::CID, const CM::Client ) = 0;
    virtual bool eraseClient(const CM::CID) = 0;
    virtual std::shared_ptr<CM::Client> copyClient(const CM::CID) const {
        return std::shared_ptr<CM::Client>();
    }
    virtual std::unique_ptr<IteratorElem<CM::Client>> itreator()=0;
    virtual Iterator<CM::Client>* getClients()=0;
};

#endif // MODEL_H
