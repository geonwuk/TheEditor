#ifndef MODEL_H
#define MODEL_H
#include "Model/type.h"
#include <memory>
#include <utility>
#include <QDebug>
template<typename T>
class Iterator{                                                         //Iterator 패턴을 위한 클래스입니다
public:
    virtual const T operator*() const =0;
    virtual void operator++() =0;
    virtual bool operator!=(Iterator& )=0;
    virtual bool operator==(Iterator& )=0;
    virtual ~Iterator(){};
};
template<typename T, typename ITR=Iterator<T>>                          //Iterator 패턴의 Iterator포인터를 감싸는 클래스입니다
struct IteratorPTR : private std::unique_ptr<ITR>{                      //unique_ptr을 씀으로써 리소스 관리를 수월하게 합니다
    IteratorPTR( ITR* itr) : std::unique_ptr<ITR>{itr} {}               //생성자의 인자로 Iterator의 포인터를 갖으며 그것을 unique_ptr에 전달합니다
    const T operator*() const{return **std::unique_ptr<ITR>::get();}
    void operator++(){ ++*std::unique_ptr<ITR>::get();}
    bool operator!=(const IteratorPTR& rhs){return *std::unique_ptr<ITR>::get()!=*rhs.get();}
    bool operator==(const IteratorPTR& rhs){return *std::unique_ptr<ITR>::get()==*rhs.get();}
};
class ClientModel{                                                                      //고객관리에 필요한 인터페이스입니다.
public:
    struct ERROR_WHILE_LOADING {unsigned int line;};
    virtual ~ClientModel(){}
    virtual unsigned int getSize() const = 0;
    virtual const CM::Client findClient(const CM::CID) const =0;
    virtual bool addClient(std::string, std::string, std::string, std::string) = 0;
    virtual void loadClient(const std::vector<CM::Client>&) noexcept(false) =0;
    virtual bool modifyClient(const CM::CID, const CM::Client) = 0;
    virtual bool eraseClient(const CM::CID) = 0;
    virtual CM::Client copyClient(const CM::CID) const = 0;                             //등록된 고객을 채팅방 참여자로 등록할 때 채팅매니저가 고객 정보가 필요하므로 이 함스로 씀으로써 고객정보를 복사합니다
    virtual void checkSafeToLoad(const std::vector<CM::Client>&) noexcept(false) =0;
    virtual IteratorPTR<CM::Client> begin() const = 0;
    virtual IteratorPTR<CM::Client> end() const = 0;
};
class ProductModel{                                                                    //상품관리에 필요한 인터페이스입니다
public:
    struct ERROR_WHILE_LOADING {unsigned int line;};
    virtual ~ProductModel(){}
    virtual bool addProduct(const std::string name, const unsigned int price, const unsigned int qty)=0;
    virtual bool loadProduct(const std::string id, const std::string name, const unsigned int price, const unsigned int qty, std::tm)=0;
    virtual void loadProduct(const std::vector<PM::Product>&) noexcept(false) =0 ;
    virtual bool modifyProduct(const PM::PID id, const PM::Product new_product)=0;
    virtual bool eraseProduct(const PM::PID id)=0;
    virtual const PM::Product findProduct(const PM::PID id) const=0;
    virtual bool buyProduct(const PM::PID id, const unsigned int qty)=0;
    virtual const unsigned int getSize() const=0;
    virtual void checkSafeToLoad(const std::vector<PM::Product>&) noexcept(false) =0;
    virtual IteratorPTR<PM::Product> begin()=0;
    virtual IteratorPTR<PM::Product> end()=0;
};
class OrderModel{
public:
    struct bill{        //주문서로 고객이 상품을 구매할 때 구매할 상품 ID와 그 갯수를 지정해야 하므로 그 정보를 갖는 클래스입니다 (addOrderView에서 이용합니다)
        PM::PID id;
        unsigned int qty;
        bill(PM::PID id,unsigned int qty):id{id},qty{qty}{}
    };
    struct ERROR_WHILE_LOADING {int line;};
    virtual ~OrderModel(){}
    virtual std::pair<const OM::Order_ID, bool> addOrder(const CM::CID client_id, std::vector<bill>)=0;
    virtual void loadOrder(const std::vector<OM::Order>&) noexcept(false) =0;
    virtual const OM::Order findOrder(const OM::Order_ID order_id) const=0;
    virtual const size_t getSize() const =0;
    virtual void checkSafeToLoad(const std::vector<OM::Order>&) noexcept(false)=0;
    virtual IteratorPTR<OM::Order> begin()=0;
    virtual IteratorPTR<OM::Order> end()=0;

};
#endif // MODEL_H
