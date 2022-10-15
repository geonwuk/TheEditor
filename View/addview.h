#ifndef ADDVIEW_H
#define ADDVIEW_H
#include "View/view.h"
#include "ui_addClient.h"
#include "ui_addProduct.h"
#include "ui_addOrder.h"

class MainWindow;
class AddClientView : public CView
{
    Q_OBJECT
public: 
    explicit AddClientView(Manager& mgr);
    ~AddClientView();
    void update() {}

public slots:
    void addClient();

private:
    Ui::addClient ui {};
};


class AddProductView : public PView
{
    Q_OBJECT
public:
    explicit AddProductView(Manager& mgr);
    ~AddProductView();
    void update() {}

public slots:
    void addProduct();

private:
    Ui::addProduct ui {};
};

class QCheckBox;
class AddOrderView : public OView
{
    Q_OBJECT
public:
    explicit AddOrderView(Manager& mgr);
    ~AddOrderView();
    void update();

public slots:
    void addOrder();
    void commitOrder();
    void cleintItemSelectionChanged_();


private:
    Ui::addOrder ui {};
    enum Page {Client=0, Product};

    QTabWidget* CPTab;
    QTableWidget clientTab;
    QTableWidget productTab;
    QTableWidget* infoTab;
    QTreeWidget* orderTree;
    void fillClientTab();
    void fillProductTab();
    void fillClientInfoTab(std::vector<CM::CID>);
    std::vector<CM::CID> getCheckedIDs(QTableWidget* table);

};



#endif // ADDVIEW_H
