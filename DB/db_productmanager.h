#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H
#include "Model/model.h"
#include <QSqlRecord>
#include <QString>
namespace DBM{

class ProductManager : public ProductModel
{
public:
    bool addProduct(const std::string name, const unsigned int price, const unsigned int qty) override;
    bool addProduct(const std::string id, const std::string name, const unsigned int price, const unsigned int qty, std::tm);
    bool modifyProduct(const PM::PID id, const PM::Product new_product) override;
    bool eraseProduct(const PM::PID id) override;
    const PM::Product findProduct(const PM::PID id) const override;
    bool buyProduct(const PM::PID id, const unsigned int qty) override;

private:
    class PIterator : public Iterator<PM::Product> {
        using Itr_type = int;
    public:
        //friend class ClientManager;
        PIterator(Itr_type p) : ptr{ p } {}
        ~PIterator() {}
        const PM::Product operator*() const override ;
        void operator++() override;
//            CIterator& operator= (Iterator& rhs) override;
        bool operator!=(Iterator& b) override;
        bool operator==(Iterator& b) override;
        QSqlRecord getPtr() const;
    private:
        Itr_type ptr;
    };

};

}
#endif // PRODUCTMANAGER_H
