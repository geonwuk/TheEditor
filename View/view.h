#ifndef VIEW_H
#define VIEW_H
#include <list>
#include "Manager/ClientManager.h"
#include "Manager/ProductManager.h"
#include "Manager/OrderManager.h"
#include "mainwindow.h"
#include "Tree.h"
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
    bool is_update=false;                               //현재 쓰지않지만 나중을 위해 만들어놓은 변수
    Manager& mgr;
    template<typename T>
    void notify(){
        mgr.notify([=](View* o){
           T* self;
           if((self=dynamic_cast<T*>(o))!=nullptr){     //만약 옵저버가 같은 타입의 뷰라면 업데이트를 실행한다
               if(self!=(T*)(this))                     //만약 옵저버가 자기 자신이라면 자기 자신에 대해서는 업데이트를 하지 않는다
                   o->update();                         //업데이트
           }
        });
    }
    QWidget* getCheckBoxWidget(QWidget* parent);
    QDateTimeEdit* getDateTimeEditWidget(const QDateTime &datetime, QWidget* parent);
    enum Role {id = Qt::UserRole};
    QTableWidgetItem* ceateTableItem(const QString id, QString title);
private:
    Tree& tree;                 //화면 왼쪽에 있는 트리
    FocusTabItem* tab=nullptr;  //화면 왼쪽에 있는 트리에서 Tabs항목에서 현재 뷰에 대한 Tabs 아이템

};

class CView : public View {
public:
    CView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, tabs, icon,label} {}
    void addClient(const QString ID, const QString name, const QString phone_number = "NONE", const QString address = "NONE");
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
    const OM::Order findOrder(const OM::Order_ID order_id) const;
    std::pair<const unsigned int, bool> addOrder(const QString client_id, std::vector<OM::OrderManager::bill>);
    const size_t getSize() const;
protected:
   // static bool is_order_moified;
};

class NView : public View {
public:
    NView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const QString label=QString()) : View{mgr, tabs, icon,label} {}
};

#endif // VIEW_H
