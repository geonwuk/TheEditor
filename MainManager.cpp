#include "MainManager.h"

#include "View/GeneralView.h"
#include "Manager/ClientManager.h"
#include "Manager/ProductManager.h"
#include "Manager/OrderManager.h"

MainManager::MainManager(MainWindow& main_window) : m_main_window{main_window}, m_server_manager{*this} {
    m_client_model = new CM::ClientManager;
    m_product_model = new PM::ProductManager;
    m_order_model = new OM::OrderManager{*m_client_model,*m_product_model};
}

MainManager::~MainManager(){
    delete m_client_model;
    m_client_model=nullptr;
    delete m_product_model;
    m_product_model=nullptr;
    delete m_order_model;
    m_order_model=nullptr;
    observers.clear();
}

void MainManager::updateAll(){          //파일을 불러오기한 경우 모두 업데이트 한다
    for (auto o : observers) {
        o->update();
    }
}

void MainManager::attachObserver(View* o){
    observers.emplace_back(o);
}
void MainManager::detachObserver(View* o){
    observers.remove(o);
}
void MainManager::reset(){
    delete m_client_model;
    m_client_model=nullptr;
    delete m_product_model;
    m_product_model=nullptr;
    delete m_order_model;
    m_order_model=nullptr;
}
