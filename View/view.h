#ifndef VIEW_H
#define VIEW_H
#include <list>
#include "Manager/ClientManager.h"
#include "Manager/ProductManager.h"
#include "Manager/OrderManager.h"
#include "mainwindow.h"

class OView;
class QTableWidgetItem;
class View : public QWidget{
public:
    virtual ~View() {mgr.detachObserver(this);}
    //virtual void show()=0;
    virtual void update()=0;
    QIcon icon;
    QString label;
protected:
    View(Manager& mgr, const QIcon icon=QPixmap(), const QString label=QString()) : icon{icon}, label{label}, mgr{mgr} {}
    bool is_update=false;
    Manager& mgr;
    template<typename T>
    void notify(){
        mgr.notify([](View* o){
           if(nullptr!=dynamic_cast<T*>(o)){
               o->update();
                qDebug()<<o->label <<"notify";
           }
        });
    }
    QWidget* getCheckBoxWidget();
    enum Role {id = Qt::UserRole};
    QTableWidgetItem* ceateTableItem(const CM::CID id, QString title);
};

class CView : public View {
public:
    CView(Manager& mgr, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, icon,label} {}
    void addClient(const QString name, const QString phone_number = "NONE", const QString address = "NONE") {
        mgr.getCM().addClient(name.toStdString(),phone_number.toStdString(),address.toStdString());
        auto c = *mgr.getCM().getCleints().begin();
        qDebug()<<c.getName().c_str()<<c.getPhoneNumber().c_str()<<c.getAddress().c_str();
        notify<CView>();
        notify<OView>();
    }
    bool eraseClient(const CM::CID id){
        bool result =  mgr.getCM().eraseClient(id);
        notify<CView>();
        notify<OView>();
        return result;
    }
    const CM::Client& findClient(const CM::CID id) const{
        return mgr.getCM().findClient(id);
    }
    CM::ClientManager::const_iterator getCleints() const{
        return mgr.getCM().getCleints();
    }
    const CM::CID getSize() const{
        return mgr.getCM().getSize();
    }
};

class PView : public View{
public:
    PView(Manager& mgr, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, icon,label} {}
    bool addProduct(const QString name, const QString price, const QString qty);
    bool eraseProduct(const PM::PID id);
    const PM::Product& findProduct(const PM::PID id) const;
    PM::ProductManager::const_iterator getProducts() const;
    const PM::Product& getProduct(const PM::PID ) const;
    const unsigned int getSize() const;
};

class OView : public View {
public:
    OView(Manager& mgr, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, icon,label} {}
    const OM::OrderManager::Order& findOrder(const OM::Order_ID order_id) const;
    std::pair<const unsigned int, bool> addOrder(const OM::Client_ID client_id, std::vector<OM::OrderManager::ProductData>);
    OM::OrderIterator getOrders() const;
    const unsigned int getSize() const;
protected:
    static bool is_order_moified;
};

#endif // VIEW_H
