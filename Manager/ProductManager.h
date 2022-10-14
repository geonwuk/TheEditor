#pragma once
#include <map>
#include <string>
#include <ctime>
#include <iosfwd>
#include <utility>
#include <fstream>
#include <vector>
namespace PM {
	using std::map;
	using std::string;
	using std::ofstream;
	using std::ifstream;
    using PID = unsigned int;

	class Product{
	public:
        Product(unsigned int id, string name, unsigned int price, unsigned int qty, std::tm date) :
			id{ id }, name{ name }, price{ price }, qty{ qty }, registered_date{ date }{}

		const unsigned int getId() const { return id; }
		const string getName() const { return name; }
		const unsigned int getPrice() const { return price; }
		const unsigned int getQty() const { return qty; }
		const std::tm getDate() const { return registered_date; }
        bool decreaseQty(const unsigned int desc){
            return qty<desc ? false : qty-=desc , true;
        }
	protected:
		Product() = default;
	private:
		unsigned int id;		
		string name;			
		unsigned int price;		
		unsigned int qty;		
		tm registered_date;		

	};
	ofstream& operator<<(ofstream& out, const Product& p);

    struct NoProduct : public Product { NoProduct(){} };
    static NoProduct no_product;
	bool operator== (const Product& p, const NoProduct&);		
	

	class ProductManager
	{
	public:
        class const_iterator;
        bool addProduct(const string name, const unsigned int price, const unsigned int qty);
		bool eraseProduct(const unsigned int id);
        Product& findProduct(const unsigned int id);
        const Product& findProduct(const unsigned int id) const;
        const_iterator getProducts() const;
		const Product& getProduct(const unsigned int) const;
		ofstream& saveProducts(ofstream&) const;
		std::pair<ifstream&, std::vector<Product>> loadProducts(ifstream&);
		const unsigned int getMaxIndex() const;
        const unsigned int getSize() const;
    private:
        static unsigned int product_id;
        map < unsigned int, Product > products;
    public:
        class const_iterator{
            using Itr_type = decltype(products)::const_iterator;
            struct Itr {
                Itr(Itr_type p) :ptr{ p } {}
                const Product& operator*() const {
                    return ptr->second;
                }
                Itr_type operator++() {
                    return ++ptr;
                }
                bool operator!=(Itr b) {
                    return ptr != b.ptr ? true : false;
                }
            private:
                Itr_type ptr;
            };
            Itr st, ed;
        public:
            const_iterator(const decltype(products)& c): st{c.begin()}, ed{c.end()} {}
            Itr begin() {return st;}
            Itr end() {return ed;}
        };

	};


}
