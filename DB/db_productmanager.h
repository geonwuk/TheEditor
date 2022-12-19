#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <ctime>

#include <string>
#include <utility>
#include <vector>

#include <QString>

#include "DB/dbmanager.h"
#include "Model/model.h"

extern const char PRODUCT_TABLE_NAME[];
/*
ProductModel 인터페이스를 구현한 클래스로 ProductModel 인터페이스에 필요한 기능을 정의합니다
메모리 대신 DB를 씀으로써 데이터를 관리하며 사용자의 입력을 DB쿼리로 변환하여 DB를 조작합니다
*/
namespace DBM{//데이터베이스와 관련된 클래스가 속한 네임스페이스입니다
class ProductManager : public ProductModel, public DBManager<PRODUCT_TABLE_NAME>
{
public:
    ProductManager(QString connection_name, QString file_name);
    bool addProduct(const std::string name, const unsigned int price, const unsigned int qty) override;
    bool loadProduct(const std::string id, const std::string name, const unsigned int price, const unsigned int qty, std::tm) override;
    void loadProduct(const std::vector<PM::Product>&) override;
    bool modifyProduct(const PM::PID id, const PM::Product new_product) override;
    bool eraseProduct(const PM::PID id) override;
    const PM::Product findProduct(const PM::PID id) const override;
    bool buyProduct(const PM::PID id, const unsigned int qty) override;
    const unsigned int getSize() const override;
    void checkSafeToLoad(const std::vector<PM::Product>&) noexcept(false) override;
    IteratorPTR<PM::Product> begin() override;
    IteratorPTR<PM::Product> end() override;
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
} //namespace DBM
#endif // PRODUCTMANAGER_H
