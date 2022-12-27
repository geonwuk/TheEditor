#ifndef ADDVIEW_H
#define ADDVIEW_H
#include "View/GeneralView.h"
#include "ui_addClient.h"
#include "ui_addProduct.h"
#include "ui_addOrder.h"

class AddClientView : public ClinetView
{
    Q_OBJECT
public: 
    explicit AddClientView(MainManager& mgr, Tree& tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~AddClientView();
    void update() {}

public slots:
    void addClient();

private:
    Ui::addClient ui {};
};

class AddProductView : public ProductView
{
    Q_OBJECT
public:
    explicit AddProductView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~AddProductView();
    void update() {}

public slots:
    void addProduct();

private:
    Ui::addProduct ui {};
};

class AddOrderView : public OrderView
{
    Q_OBJECT
public:
    explicit AddOrderView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~AddOrderView();
    void update();
public slots:
    void addOrder();
    void commitOrder();
    void itemSelectionChanged_();
private:
    Ui::addOrder ui {};
    enum Page {Client=0, Product};

    QTabWidget* CPTab;          //ui에서 삭제
    QTableWidget* infoTab;      //ui에서 삭제
    QTreeWidget* orderTree;     //ui에서 삭제
    QShortcut* shortcut;        //소멸자에서 삭제
    QTableWidget clientTab;
    QTableWidget productTab;

    void fillClientTab();
    void fillProductTab();
    void fillClientInfoTab(const std::list<std::string>);
    void fillProductInfoTab(const std::list<std::string>);
    std::vector<std::string> getCheckedIDs(QTableWidget* table);
};

#include "ui_addParticipantView.h"
class AddParticipantView : public NetworkView{
    Q_OBJECT
public:
    explicit AddParticipantView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~AddParticipantView();
    void update();
private:
    Ui::addParticipantView ui;
    void fillContents();
    void initUI();
private slots:
    void addParticipant();
    void dropParticipant();
};

#endif // ADDVIEW_H
