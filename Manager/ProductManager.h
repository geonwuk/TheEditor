#pragma once
#include <map>
#include <string>
#include <ctime>
#include <iosfwd>
#include <utility>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Model/model.h"
namespace PM {
	using std::map;
	using std::string;
	using std::ofstream;
	using std::ifstream;

    struct ERROR_WHILE_LOADING { const unsigned int line; };

    ofstream& operator<<(ofstream& out, const Product& p);
	

    class ProductManager : public ProductModel{
	public:
        bool addProduct(const string name, const unsigned int price, const unsigned int qty) override;
        bool loadProduct(const string id, const string name, const unsigned int price, const unsigned int qty, std::tm) override;
        bool modifyProduct(const PID id, const Product new_product) override;
        bool eraseProduct(const PID id) override;
        const Product findProduct(const PID id) const override;
        bool buyProduct(const PID id, const unsigned int qty) override;
        ifstream& loadProducts(ifstream&, unsigned int);
        const unsigned int getSize() const override;
    private:
        static unsigned int product_id;                                     //랜덤 ID를 생성할 때 쓰는 seed격 상수입니다
        map < std::string, Product > products;                              //상품들을
        string generateRandID(tm time);                                     //상품 ID는 자동으로 랜덤하게 생성하며 product_id와 시각 정보를 합쳐 랜덤 ID를 생성하기 위한 SEED로 씁니다
        class PIterator : public Iterator<Product>{
        public:
            using Itr_type = decltype(products)::const_iterator;
            PIterator(Itr_type p) :ptr{ p } {}
            const Product operator*() const override{
                return ptr->second;
            }
            void operator++() override{
                ++ptr;
            }
            bool operator!=(Iterator& rhs) override {
                return (ptr != static_cast<PIterator&>(rhs).ptr ? true : false);
            }
            bool operator==(Iterator& rhs) override {
                return (ptr == static_cast<PIterator&>(rhs).ptr ? true : false);
            }
        private:
            Itr_type ptr;
        };
    public:
        IteratorPTR<PM::Product> begin() override {
            return IteratorPTR<PM::Product>(new PIterator{products.begin()});
        }
        IteratorPTR<PM::Product> end() override {
            return IteratorPTR<PM::Product>(new PIterator{products.end()});
        }
	};


}
