#ifndef MODEL_H
#define MODEL_H
#include "Model/type.h"
#include <memory>
template<typename T>
class IteratorElem{
public:
    virtual const T& operator*() const;
    virtual void operator++();
    virtual bool operator!=(IteratorElem& );
    virtual bool operator==(IteratorElem& );
    virtual T* operator->();
};

template<typename T>
struct Iterator{
    virtual IteratorElem<T>& begin()=0;
    virtual IteratorElem<T>& end()=0;
};
using std::string;
class ClientModel
{
public:
    virtual unsigned int getSize() const = 0;
    virtual Iterator<CM::Client>& getClients() const = 0;
    virtual const CM::Client& findClient(const CM::CID) const =0;
    virtual void addClient(string, string, string, string) = 0;
    virtual bool modifyClient(const CM::CID, const CM::Client ) = 0;
    virtual bool eraseClient(const CM::CID) = 0;
    virtual std::shared_ptr<CM::Client> copyClient(const CM::CID) const = 0;

};

#endif // MODEL_H
