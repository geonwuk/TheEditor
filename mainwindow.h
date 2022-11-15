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
    friend class MainWindow;
public:
    Manager(MainWindow& mw) : mw{mw} {
        cm = new DBM::ClientManager{"client"};
        pm = new DBM::ProductManager{"product"};
        om = new DBM::OrderManager{*cm,*pm,"orders"};
    }
    template<typename F>
    void notify(F notify_){
        for (auto o : observers) {
            notify_(o);
        }
    }
    void updateAll();
    ClientModel& getCM(){
        return *cm;
    }
    ProductModel& getPM(){
        return *pm;
    }
    OrderModel& getOM(){
        return *om;
    }
    ServerManager& getSM(){
        return sm;
    }
    void attachObserver(View* o);
    void detachObserver(View* o);
    void changeToDB();
    void changeToMemory();
    void reset();

private:
    std::list<View*> observers;
    MainWindow& mw;
    ClientModel* cm;
    ProductModel* pm;
    OrderModel* om;
    ServerManager sm{*this};
};

class MainWindow : public QMainWindow
{
    friend class Manager;
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Manager* getMgr(){return &mgrs;}
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
