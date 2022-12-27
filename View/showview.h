#ifndef SHOWVIEW_H
#define SHOWVIEW_H
#include "View/GeneralView.h"
#include "ui_showClient.h"
#include "ui_showOrder.h"
#include "MainManager.h"


class QCheckBox;
class MainWindow;

class ShowClientView : public ClinetView
{
    Q_OBJECT
public:
    explicit ShowClientView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~ShowClientView();
    void update();
private:
    void fillContents();
    bool is_edit_mode;       //읽기 수정 모드 설정 변수
    Ui::showClient ui{};
    QTableWidget* table;        //ui가 삭제
    QCheckBox* editBox;         //ui가 삭제
    QLineEdit* searchLineEdit;  //ui가 삭제
    bool eraseClient(int row);
    const int id_col;
    QShortcut* shortcut;            //delete키를 누르면 그행이 삭제되는 단축키
private slots:
    void cellChanged(int,int);
    void returnPressed();
};


class ShowProductView : public ProductView
{
    Q_OBJECT
public:
    explicit ShowProductView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~ShowProductView();
    void update();
private:
    Ui::showClient ui {};
    QTableWidget* table;        //ui가 삭제
    QCheckBox* editBox;         //ui가 삭제
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

class ShowOrderView : public OrderView
{
    Q_OBJECT
public:
    explicit ShowOrderView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~ShowOrderView();
    void update();
public slots:
    void orderItemSelectionChanged_();

private:
    Ui::showOrder ui {};
    QTableWidget *orderTable;       //ui 자원이므로 ui에서 삭제
    QTableWidget *orderInfoTable;   //ui 자원이므로 ui에서 삭제
    void fillContents();
};


class QMenu;
class QProgressDialog;
class ServerManager;
#include "ui_chatRoom.h"
#include "Network/logthread.h"
#include "Network/logmanagement.h"
class ShowChatView : public NetworkView
{
    Q_OBJECT
public:
    ShowChatView(MainManager& mgr, Tree &tabs, const QIcon icon=QPixmap(), const std::string label=std::string());
    ~ShowChatView();
    Ui::chatRoom ui;
    void update();
    void clientLogin();
    void addLog(const ServerManager::ChatMessage& );
private:
    void fillclientTree();
    LogThread* log_thread;              //소멸자에서 삭제
    QProgressDialog* progressDialog;    //소멸자에서 삭제
    QMenu* menu;                        //소멸자에서 삭제
    ServerManager& smgr;
    QAction* removeAction;              //소멸자에서 삭제
    QPushButton* addCosumerButton;      //ui에서 삭제
    QPushButton* deleteConsumerButton;  //ui에서 삭제
    QTreeWidget* consumerTreeWidget;    //ui에서 삭제
    std::list<LogConsumer> consumers;
    LogBroker broker;
    LogProducer producer;
private slots:
    void on_clientTreeWidget_customContextMenuRequested(const QPoint &pos);
    void savePressed();
    void addConsumer();
    void delteConsumer();
};

#endif // SHOWVIEW_H
