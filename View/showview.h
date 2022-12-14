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
    bool is_edit_mode =false;       //읽기 수정 모드 설정 변수
    Ui::showClient ui{};
    QTableWidget* table;
    QCheckBox* editBox;
    QLineEdit* searchLineEdit;
    bool eraseClient(int row);
    const int id_col=0;
    QShortcut* shortcut;            //delete키를 누르면 그행이 삭제되는 단축키

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
    bool is_edit_mode =false;       //읽기 수정 모드 설정 변수
    QLineEdit* searchLineEdit;
    bool eraseProduct(int row);
    void fillContents();
    const int id_col=0;
    QShortcut* shortcut;            //delete키를 누르면 그행이 삭제되는 단축키
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
#include "Network/logthread.h"
#include "Network/logmanagement.h"
class ShowChatView : public NView
{
    Q_OBJECT
    LogThread* log_thread;
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

    QPushButton* addCosumerButton;
    QPushButton* deleteConsumerButton;
    QTreeWidget* consumerTreeWidget;
    std::list<LogConsumer> consumers;
    LogBroker broker;
    LogProducer producer{broker};
private slots:
    void on_clientTreeWidget_customContextMenuRequested(const QPoint &pos);
    void savePressed();

    void addConsumer();
    void delteConsumer();
};

#endif // SHOWVIEW_H
