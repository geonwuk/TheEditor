#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Manager/ClientManager.h"
#include "Manager/ProductManager.h"
#include "Manager/OrderManager.h"
#include "tabwidget.h"
#include "tree.h"
#include <list>
#include "Network/servermanager.h"
#include "Model/model.h"
#include "DB/db_clientmanager.h"
#include "DB/db_productmanager.h"
#include "DB/db_ordermanager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;
             class dashboard;}
QT_END_NAMESPACE

class MainWindow;
class View;
class Tree;
class QStackedWidget;
class Manager {
public:
    Manager(MainWindow& mw) : mw{mw} {}

    template<typename F>
    void notify(F notify_){
        for (auto o : observers) {
            notify_(o);
        }
    }

    void updateAll();

    ClientModel& getCM(){
        return cm2;
    }
    ProductModel& getPM(){
        return pm2;
    }
    OrderModel& getOM(){
        return om3;
    }
    ServerManager& getSM(){
        return sm;
    }

    void attachObserver(View* o);
    void detachObserver(View* o){
        observers.remove(o);
    }
private:
    std::list<View*> observers;
    MainWindow& mw;
    CM::ClientManager cm;
    DBM::ClientManager cm2{"cleint"};

    PM::ProductManager pm;
    DBM::ProductManager pm2{"product"};
        DBM::OrderManager om3{cm2,pm2,"orders"};
    OM::OrderManager om{cm,pm};

    ServerManager sm{*this};
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Manager* getMgr(){return &mgrs;}
    bool is_dirty=false;

private slots:
    void save();
    void load();
    void onRadioButtonDBClicked();
    void onRadioButtonMemoryClicked();

private:
    Ui::MainWindow *ui;
    Ui::dashboard* dash_board;
    Manager mgrs{*this};
    TabWidget management_tw{this};
    ManagementTree management_tree{this,&management_tw};

    TabWidget network_tw{this};
    NetworkTree network_tree{this,&network_tw};

    QStackedWidget* sw;





};
#endif // MAINWINDOW_H
