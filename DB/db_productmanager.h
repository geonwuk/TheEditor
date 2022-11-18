#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H
#include "Model/model.h"
#include <QSqlRecord>
#include <QString>
#include "DB/dbmanager.h"
extern const char PRODUCT_TABLE_NAME[];
namespace DBM{//데이터베이스와 관련된 클래스가 속한 네임스페이스입니다
class ProductManager : public ProductModel, public DBManager<PRODUCT_TABLE_NAME>
{
public:
    ProductManager(QString connection_name);
    ProductManager(QString connection_name, QString file_name):DBManager{connection_name, file_name} {}
    bool addProduct(const std::string name, const unsigned int price, const unsigned int qty) override;
    bool loadProduct(const std::string id, const std::string name, const unsigned int price, const unsigned int qty, std::tm) noexcept(false) override;
    bool modifyProduct(const PM::PID id, const PM::Product new_product) override;
    bool eraseProduct(const PM::PID id) override;
    const PM::Product findProduct(const PM::PID id) const override;
    bool buyProduct(const PM::PID id, const unsigned int qty) override;
    virtual const unsigned int getSize() const override;
    virtual IteratorPTR<PM::Product> begin() override;
    virtual IteratorPTR<PM::Product> end() override;
private:
    std::string generateRandID(tm time);
    static unsigned int product_id;
private:
    class PIterator : public DBIterator<PM::Product> {
    public:
        using DBIterator::DBIterator;
        const PM::Product operator*() const override ;
    };
};

}
#endif // PRODUCTMANAGER_H
