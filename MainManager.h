#ifndef MAINMANAGER_H
#define MAINMANAGER_H

#include <list>

#include "Network/servermanager.h"
#include "Model/Model.h"

class View;
class MainWindow;
class View;
class QStackedWidget;

class MainManager {
    friend class MainWindow;
public:
    MainManager(MainWindow& mw);
    ~MainManager();
    template<typename F>
    void notify(F notify_){
        for (auto o : observers) {
            notify_(o);
        }
    }
    void updateAll();
    ClientModel& getClientModel() { return *m_client_model; }
    ProductModel& getProductModel() { return *m_product_model; }
    OrderModel& getOrderModel() { return *m_order_model; }
    ServerManager& getServerManager() { return m_server_manager; }
    void attachObserver(View* o);
    void detachObserver(View* o);
    void reset();
private:
    std::list<View*> observers;
    MainWindow& m_main_window;
    ClientModel* m_client_model;                //소멸자에서 삭제
    ProductModel* m_product_model;               //소멸자에서 삭제
    OrderModel* m_order_model;                 //소멸자에서 삭제
    ServerManager m_server_manager;
};
#endif // MAINMANAGER_H
