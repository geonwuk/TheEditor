#ifndef SHOWVIEW_H
#define SHOWVIEW_H
#include "View/view.h"
#include "ui_showClient.h"
#include "ui_showOrder.h"

class QCheckBox;
class MainWindow;

class ShowClientView : public CView
{
    Q_OBJECT
public:
    explicit ShowClientView(Manager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const QString label=QString());
    ~ShowClientView();
    void update();

private:
    void fillContents();
    bool is_edit_mode =false;
    Ui::showClient ui{};
    QTableWidget* table;
    QCheckBox* editBox;
    QLineEdit* searchLineEdit;
    bool eraseClient(int row);
    const int id_col=0;

private slots:
    void cellChanged(int,int);
    void returnPressed();
};


class ShowProductView : public PView
{
    Q_OBJECT
public:
    explicit ShowProductView(Manager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const QString label=QString());
    ~ShowProductView();
    void update();
private:
    using UI_PRODUCT = Ui::showClient;
    UI_PRODUCT ui {};
    QTableWidget* table;
    QCheckBox* editBox;
    bool is_edit_mode =false;
    QLineEdit* searchLineEdit;
    bool eraseProduct(int row);
    void fillContents();
    const int id_col=0;
private slots:
    void cellChanged(int,int);
    void returnPressed();
};

class ShowOrderView : public OView
{
    Q_OBJECT
public:
    explicit ShowOrderView(Manager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const QString label=QString());
    ~ShowOrderView();
    void update();
public slots:
    void orderItemSelectionChanged_();

private:
    Ui::showOrder ui {};
    QTableWidget *orderTable;
    QTableWidget *orderInfoTable;
    void fillContents();
};


class QMenu;
class QProgressDialog;
class ServerManager;
#include "ui_chatRoom.h"
class ShowChatView : public NView
{
    Q_OBJECT
    QProgressDialog* progressDialog;
    QMenu* menu;
//    std::vector<std::shared_ptr<NetClient>> participants;
public:
    ShowChatView(Manager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const QString label=QString());
    ~ShowChatView();
    Ui::chatRoom ui;
    void update();
    void clientLogin();
    void addLog(const ServerManager::ChatMessage& );

private:
    void fillclientTree();
    ServerManager& smgr;

private slots:
    void on_clientTreeWidget_customContextMenuRequested(const QPoint &pos);
};

#endif // SHOWVIEW_H
