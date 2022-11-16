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
        static unsigned int product_id;
        map < std::string, Product > products;
        string generateRandID(tm time);
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
