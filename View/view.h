#ifndef VIEW_H
#define VIEW_H

#include <list>
#include <string>

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
    virtual void update()=0;
    QIcon icon;
    std::string label;
    void removeFromTree();
protected:
    View(Manager& mgr, Tree& tree, const QIcon& icon, const std::string label);
    bool is_update;                               //현재 쓰지않지만 나중을 위해 만들어놓은 변수
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
    QTableWidgetItem* ceateTableItem(const std::string id, std::string title);
private:
    Tree& tree;                 //화면 왼쪽에 있는 트리
    FocusTabItem* tab;  //소멸자에서 삭제, 화면 왼쪽에 있는 트리에서 Tabs항목에서 현재 뷰에 대한 Tabs 아이템
};

class CView : public View {
public:
    CView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const std::string label=std::string()) : View{mgr, tabs, icon,label} {}
    void addClient(const std::string ID, const std::string name, const std::string phone_number = "NONE", const std::string address = "NONE");
    bool eraseClient(const std::string id);
    bool modifyClient(const std::string id, const QList<std::string> client_info);
    const CM::Client findClient(const CM::CID id) const{
        return mgr.getCM().findClient(id);
    }
    const unsigned int getSize() const{
        return mgr.getCM().getSize();
    }
};

class PView : public View{
public:
    PView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const std::string label=std::string()) : View{mgr, tabs, icon,label} {}
    bool addProduct(const std::string name, const std::string price, const std::string qty);
    bool modifyProduct(const std::string id, const QList<std::string> ls);
    bool eraseProduct(const std::string id);
    const PM::Product findProduct(const std::string id) const;
    const unsigned int getSize() const;
};

class OView : public View {
public:
    OView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const std::string label=std::string()) : View{mgr, tabs, icon,label} {}
    const OM::Order findOrder(const OM::Order_ID order_id) const;
    std::pair<const unsigned int, bool> addOrder(const std::string client_id, std::vector<OM::OrderManager::bill>);
    const size_t getSize() const;
protected:
   // static bool is_order_moified;
};

class NView : public View {
public:
    NView(Manager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const std::string label=std::string()) : View{mgr, tabs, icon,label} {}
};

#endif // VIEW_H
