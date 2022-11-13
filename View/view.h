#ifndef VIEW_H
#define VIEW_H
#include <list>
#include "Manager/ClientManager.h"
#include "Manager/ProductManager.h"
#include "Manager/OrderManager.h"
#include "mainwindow.h"
#include "tree.h"
class OView;
class QTableWidgetItem;
class FocusTabItem;
class NView;
class QDateTimeEdit;
class View : public QWidget{
public:
    virtual ~View();
    //virtual void show()=0;
    virtual void update()=0;
    QIcon icon;
    QString label;
    void removeFromTree();
protected:
    View(Manager& mgr, Tree& tree, const QIcon& icon=QPixmap(), const QString label=QString());
    bool is_update=false;
    Manager& mgr;
    template<typename T>
    void notify(){
        mgr.notify([=](View* o){
           T* self;
           if((self=dynamic_cast<T*>(o))!=nullptr){
               if(self!=(T*)(this))
                   o->update();
                //qDebug()<<o->label <<"notify";
           }
        });
    }
    QWidget* getCheckBoxWidget(QWidget* parent);
    QDateTimeEdit* getDateTimeEditWidget(const QDateTime &datetime, QWidget* parent);
    enum Role {id = Qt::UserRole};
    QTableWidgetItem* ceateTableItem(const QString id, QString title);
private:
    Tree& tree;
    FocusTabItem* tab=nullptr;

};

class CView : public View {
public:
    CView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, tabs, icon,label} {}
    void addClient(const QString ID, const QString name, const QString phone_number = "NONE", const QString address = "NONE") {
        mgr.getCM().addClient(ID.toStdString(), name.toStdString(),phone_number.toStdString(),address.toStdString());
        notify<CView>();
        notify<OView>();
        notify<NView>();
    }
    bool eraseClient(const QString id);
    bool modifyClient(const QString id, const QList<QString> client_info);

    const CM::Client findClient(const CM::CID id) const{
        return mgr.getCM().findClient(id);
    }
    const unsigned int getSize() const{
        return mgr.getCM().getSize();
    }
};

class PView : public View{
public:
    PView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, tabs, icon,label} {}
    bool addProduct(const QString name, const QString price, const QString qty);
    bool modifyProduct(const QString id, const QList<QString> ls);
    bool eraseProduct(const QString id);
    const PM::Product findProduct(const QString id) const;
    const unsigned int getSize() const;
};

class OView : public View {
public:
    OView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, tabs, icon,label} {}
    const OM::Order& findOrder(const OM::Order_ID order_id) const;
    std::pair<const unsigned int, bool> addOrder(const QString client_id, std::vector<OM::OrderManager::bill>);
    const size_t getSize() const;
protected:
    static bool is_order_moified;
};

class NView : public View {
public:
    NView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, tabs, icon,label} {}
};

#endif // VIEW_H
