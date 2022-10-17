#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Manager/ClientManager.h"
#include "Manager/ProductManager.h"
#include "Manager/OrderManager.h"
#include "tabwidget.h"
#include "tree.h"
#include <list>
#include "Network/server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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

    CM::ClientManager& getCM(){
        return cm;
    }
    PM::ProductManager& getPM(){
        return pm;
    }
    OM::OrderManager& getOM(){
        return om;
    }
    ServerManager& getSM(){
        return sm;
    }

    void attachObserver(View* o);
    void detachObserver(View* o){
        qDebug()<<"detach";
        observers.remove(o);
    }
private:
    std::list<View*> observers;
    MainWindow& mw;
    CM::ClientManager cm;
    PM::ProductManager pm;
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

public slots:
    void treeToTab(QWidget *page, const QIcon &icon, const QString &label);
    void setTabFocus(QWidget* page);
private:
    Ui::MainWindow *ui;
    Manager mgrs{*this};

    TabWidget management_tw{this};
    ManagementTree management_tree{this,&management_tw};

    TabWidget network_tw{this};
    NetworkTree network_tree{this,&network_tw};

    QStackedWidget* sw;



};
#endif // MAINWINDOW_H
