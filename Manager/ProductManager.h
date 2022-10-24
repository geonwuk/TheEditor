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
namespace PM {
	using std::map;
	using std::string;
	using std::ofstream;
	using std::ifstream;
    using PID = string;
    class ProductManager;
	class Product{
	public:
        Product(string name, unsigned int price, unsigned int qty) :
            name{ name }, price{ price }, qty{ qty } {}
        Product(string id, string name, unsigned int price, unsigned int qty, std::tm date) :
            id{ id }, name{ name }, price{ price }, qty{ qty }, registered_date{ date }{}
        const string getId() const { return id; }
		const string getName() const { return name; }
		const unsigned int getPrice() const { return price; }
		const unsigned int getQty() const { return qty; }
		const std::tm getDate() const { return registered_date; }
	protected:
		Product() = default;
	private:
        friend class ProductManager;
        string id;
		string name;			
		unsigned int price;		
		unsigned int qty;		
        tm registered_date;
        bool decreaseQty(const unsigned int desc){
            return qty<desc ? false : qty-=desc , true;
        }
        Product& operator= (const Product& rhs){
            name=rhs.name;
            price=rhs.price;
            qty=rhs.qty;
            return *this;
        }

	};
	ofstream& operator<<(ofstream& out, const Product& p);

    struct NoProduct : public Product { NoProduct(){} };
    const NoProduct no_product;
	bool operator== (const Product& p, const NoProduct&);		
	

	class ProductManager
	{
	public:
        class const_iterator;
        bool addProduct(const string name, const unsigned int price, const unsigned int qty);
        bool addProduct(const string id, const string name, const unsigned int price, const unsigned int qty, std::tm);
        bool modifyProduct(const PID id, const Product new_product);
        bool eraseProduct(const PID id);
        Product& findProduct(const PID id);
        const Product& findProduct(const PID id) const;
        std::shared_ptr<Product> copyProduct(const PID) const noexcept;
        const_iterator getProducts() const;
        bool buyProduct(const PID id, const unsigned int qty);
		ofstream& saveProducts(ofstream&) const;
		std::pair<ifstream&, std::vector<Product>> loadProducts(ifstream&);
        const unsigned int getSize() const;
    private:
        static unsigned int product_id;
        map < std::string, std::shared_ptr<Product> > products;
        string generateRandID(tm time);
    public:
        class const_iterator{
            using Itr_type = decltype(products)::const_iterator;
            struct Itr {
                Itr(Itr_type p) :ptr{ p } {}
                const Product& operator*() const {
                    return *ptr->second.get();
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
