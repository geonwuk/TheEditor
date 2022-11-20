#include "mainwindow.h"
#include <QLabel>
#include <QList>
#include "ui_mainwindow.h"
#include "tree.h"
#include "View/view.h"
#include <QSplitter>
#include "Network/server.h"
#include <QFileDialog>
#include <fstream>
#include <QMessageBox>
#include "ui_dashboard.h"
#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include<QMessageBox>
using namespace std;

static QSplitter* initTreeAndTab(Tree& tree, TabWidget& tw){        //트리와 탭 화면을 스플리터로 나누는 함수
    QSplitter* splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);
    QSizePolicy tree_policy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    tree_policy.setHorizontalStretch(0);                                        //화면이 확대될 때 트리는 가만히 있어야 함
    tree.setSizePolicy(tree_policy);                                            //Maximum 사이즈 정책 적용
    tree.setMaximumSize(1920,1920);                                             //수동으로 스플리터를 움직여서 트리 크기를 조절할 수 있도록 최대 크기를 일반적인 1920픽셀로 설정한다
    splitter->addWidget(&tree);
    QSizePolicy tab_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tab_policy.setHorizontalStretch(1);                                       //화면이 확대될 때 트리는 가만히, 탭 화면만 확대되야 함
    tw.setSizePolicy(tab_policy);                                               //Expanding 사이즈 정책 적용
    splitter->addWidget(&tw);                                                   //스플리터에 추가
    tw.setTabsClosable(true);
    return splitter;
}
#include <QStyleFactory>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dash_board(new Ui::dashboard)
{
    ui->setupUi(this);
    sw=ui->stackedWidget;


    ui->actionSave->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogSaveButton));      //save 액션 아이콘 추가
    ui->actionOpen->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogOpenButton));      //load 액션 아이콘 추가

    QSplitter* splitter = initTreeAndTab(management_tree,management_tw);                     //고객,물품,주문관리 splitter
    QSplitter* splitter2 = initTreeAndTab(network_tree, network_tw);                        //네트워크 splitter

    sw->addWidget(splitter);         //고객,물품,주문관리
    sw->addWidget(splitter2);        //네트워크
    QWidget* dash_board_widget = new QWidget(this);
    dash_board->setupUi(dash_board_widget);
    sw->addWidget(dash_board_widget);
    connect(ui->ManagementButton, &QToolButton::pressed, this, [=]{ sw->setCurrentIndex(0);}); //고객,물품,주문관리
    connect(ui->ChatButton, &QToolButton::pressed, this, [=]{ sw->setCurrentIndex(1); });      //네트워크
    connect(ui->memoryButton, &QToolButton::pressed, this, [=]{ sw->setCurrentIndex(2); });      //메모리

    mgrs.getSM().setServer(new Server{mgrs.getSM()});                                   //서버 생성 후 포인터로 설정

    connect(ui->actionSave,SIGNAL(triggered()),SLOT(save()));       //저장
    connect(ui->actionOpen,SIGNAL(triggered()),SLOT(load()));       //불러오기
    setWindowTitle(tr("Program"));

    connect(dash_board->radioButtonDB, SIGNAL(clicked()), this, SLOT(onRadioButtonDBClicked()));
    connect(dash_board->radioButtonMemory, SIGNAL(clicked()), this, SLOT(onRadioMemoryButtonClicked()));
}


void Manager::updateAll(){          //파일을 불러오기한 경우 모두 업데이트 한다
    for (auto o : observers) {
        o->update();
    }
}

void MainWindow::onRadioButtonDBClicked(){
    try{
        QString file_name = QFileDialog::getSaveFileName(this,tr("Select New DB File"));
        if(file_name.size()==0){
            dash_board->groupBox->blockSignals(true);
            dash_board->radioButtonMemory->click();
            dash_board->groupBox->blockSignals(false);
            return;
        }
        try{
            DBM::ClientManager save_client{"save_client",file_name};
            for(const auto& c : mgrs.getCM()){
                bool re = save_client.addClient(c.getId(),c.getName(),c.getPhoneNumber(),c.getAddress());
                if(!re)
                    throw ClientModel::ERROR_WHILE_LOADING{};
            }
            DBM::ProductManager save_product{"save_product",file_name};
            for(const auto& p : mgrs.getPM()){
                save_product.loadProduct(p.getId(),p.getName(),p.getPrice(),p.getQty(),p.getDate());
            }
            DBM::OrderManager save_order{mgrs.getCM(),mgrs.getPM(),"save_order",file_name};
            vector<OM::Order> orders;
            for(const auto& o : mgrs.getOM()){
                orders.emplace_back(o);
            }
            save_order.loadOrder(orders);
        }
        catch(...){
            QMessageBox::critical(this, tr("ERROR WHILE MAKING DB Memory"), tr("Can't change memory model to DB"));
            dash_board->groupBox->blockSignals(true);
            dash_board->radioButtonMemory->click();
            dash_board->groupBox->blockSignals(false);
            return;
        }
        auto dbcm = new DBM::ClientManager{"client",file_name};
        auto dbpm = new DBM::ProductManager {"product",file_name};
        auto dbom = new DBM::OrderManager {*dbcm,*dbpm,"order",file_name};
        mgrs.reset();
        mgrs.cm = dbcm;
        mgrs.pm = dbpm;
        mgrs.om = dbom;
        auto db_path_item = dash_board->tableWidgetDB->item(2,0);
        db_path_item->setText(file_name);
        QFileInfo fi{file_name};
        dash_board->tableWidgetDB->item(1,0)->setText(fi.fileName());
        dash_board->tableWidgetDB->item(0,0)->setText("SQLite");
    }
    catch(...){
        QMessageBox::critical(this, tr("FAIL"), tr("Can't change memory model to DB"));
        dash_board->groupBox->blockSignals(true);
        dash_board->radioButtonMemory->click();
        dash_board->groupBox->blockSignals(false);
    }
    mgrs.updateAll();//로드한 후 모든 뷰(탭)를 갱신합니다. 디버깅 용도입니다
}
void MainWindow::onRadioMemoryButtonClicked(){
    try{
        auto mcm = new CM::ClientManager;
        auto mpm = new PM::ProductManager;
        auto mom = new OM::OrderManager{*mcm, *mpm};

        for(const auto& c : mgrs.getCM()){
            bool re = mcm->addClient(c.getId(),c.getName(),c.getPhoneNumber(),c.getAddress());
            if(!re)
                throw ClientModel::ERROR_WHILE_LOADING{};
        }
        for(const auto& p : mgrs.getPM()){
            bool re = mpm->loadProduct(p.getId(),p.getName(),p.getPrice(),p.getQty(),p.getDate());
            if(!re)
                throw ProductModel::ERROR_WHILE_LOADING{};
        }
        vector<OM::Order> orders;
        for(const auto& o : mgrs.getOM()){
            orders.emplace_back(o);
        }
        mom->loadOrder(orders);

        mgrs.reset();
        mgrs.cm = mcm;
        mgrs.pm = mpm;
        mgrs.om = mom;
        auto db_path_item = dash_board->tableWidgetDB->item(2,0);
        db_path_item->setText(tr("NONE"));
        dash_board->tableWidgetDB->item(1,0)->setText(tr("NONE"));
        dash_board->tableWidgetDB->item(0,0)->setText("NONE");
    }
    catch(...){
        QMessageBox::critical(this, tr("FAIL"), tr("Can't change memory model to memory"));
        dash_board->groupBox->blockSignals(true);
        dash_board->radioButtonDB->click();
        dash_board->groupBox->blockSignals(false);
    }
    mgrs.updateAll();//로드한 후 모든 뷰(탭)를 갱신합니다. 디버깅 용도입니다
}

static int createListDialog(QWidget* parent, QStringList ls){       //저장이나 불러오기를 할 때 DB, CSV에서 사용자가 선택할 수 있게 다이어로그를 만드는 함수입니다
    std::unique_ptr<QDialog> dialog {new QDialog(parent)};
    dialog->setModal(true);
    QListWidget* list = new QListWidget(dialog.get());
    for(const auto& e : ls){
        list->addItem(e);
    }
    QPushButton* ok= new QPushButton("OK",dialog.get());
    QPushButton* cancel = new QPushButton("cancel",dialog.get());
    QVBoxLayout* v_layout = new QVBoxLayout;
    v_layout->addWidget(list);
    v_layout->addWidget(ok);
    v_layout->addWidget(cancel);
    dialog->setLayout(v_layout);
    QObject::connect(ok,SIGNAL(clicked()),dialog.get(),SLOT(accept()));
    QObject::connect(cancel,SIGNAL(clicked()),dialog.get(),SLOT(reject()));
    dialog->exec();
    int result = dialog->result()!=QDialog::Accepted ? -1 : list->currentRow();
    return result;
}

void MainWindow::save(){                                                //파일을 sqlite DB로 저장하는 함수로 QAction과 연결되어 있다
    QString file_name = QFileDialog::getSaveFileName(this);
    if(file_name.size()==0){
        return;
    }
    int mode = createListDialog(this,{"DB","CSV"});
    qDebug()<<"ee"<<file_name<<mode;
    if(mode==0){ //DB
        try{
            DBM::ClientManager save_client{"save_client",file_name};
            for(const auto& c : mgrs.getCM()){
                bool re = save_client.addClient(c.getId(),c.getName(),c.getPhoneNumber(),c.getAddress());
                if(!re)
                    throw -1;
            }
            DBM::ProductManager save_product{"save_product",file_name};
            for(const auto& p : mgrs.getPM()){
                save_product.loadProduct(p.getId(),p.getName(),p.getPrice(),p.getQty(),p.getDate());
            }
            DBM::OrderManager save_order{mgrs.getCM(),mgrs.getPM(),"save_order",file_name};
            vector<OM::Order> orders;
            for(const auto& o : mgrs.getOM()){
                orders.emplace_back(o);
            }
            save_order.loadOrder(orders);
        }
        catch(...){

        }
    }
    else if(mode==1){ //CSV
        try{
            std::ofstream out(file_name.toStdString());
            int mode = createListDialog(this,{"client","product","order"});
            switch(mode){
            case 0://client CSV로 export
                out<<"client id,"<<"name,"<<"phone number,"<<"address"<<','<<endl;
                for (const auto& c : mgrs.getCM()) {
                    out << c <<',' << endl;
                }
                break;
            case 1://product CSV로 export
                out<<"product id,"<<"name,"<<"qty,"<<"date"<<','<<endl;
                for (const auto& p : mgrs.getPM()){
                    out << p <<',' << endl;
                }
                break;
            case 2://order CSV로 export
                out<<"order id,"<<"buyer,"<<"date,"<<"product id,"<<"product name,"<<"product qty"<<"product date,"<<"..."<<endl;
                for (const auto& o : mgrs.getOM()){
                    out << o <<',' << endl;
                }
                break;
            default:
                break;
            };
        }
        catch(...){

        }

    }
}
void MainWindow::load(){
    //저장된 파일을 불러오는 함수로 QAction과 연결되어 있다
    try{
        QString file_name = QFileDialog::getSaveFileName(this);
        if(file_name.size()==0){
            return;
        }
        int mode = createListDialog(this,{"DB"});

        switch(mode){
        case 0://DB로부터 데이터 로드
        {
            DBM::ClientManager load_client{"load_client",file_name};
            DBM::ProductManager load_product{"load_product",file_name};
            DBM::OrderManager load_order{load_client,load_product,"load_order",file_name};
            vector<CM::Client> clients;
            for(const auto& c : load_client){
                clients.emplace_back(c);
            }
            vector<PM::Product> products;
            for(const auto& p : load_product){
                products.emplace_back(p);
            }
            vector<OM::Order> orders;
            for(const auto& o : load_order){
                orders.emplace_back(o);
            }
            mgrs.getCM().loadClient(clients);
            mgrs.getPM().loadProduct(products);
            mgrs.getOM().loadOrder(orders);
        }
            break;
        default:
            break;
        }
    }
    catch(DBM::ERROR_WHILE_LOADING err){
        QString msg{"Please select correct DB file"};
        msg+=QString(err.db_name.c_str());
        QMessageBox::critical(this, tr("FAIL LOADING"), tr(msg.toStdString().c_str()));
    }
    catch(ClientModel::ERROR_WHILE_LOADING err){
        QString msg{"Duplicate ID found while loading client data at line number of "};
        msg+=QString::number(err.line);
        QMessageBox::critical(this, tr("FAIL LOADING"), tr(msg.toStdString().c_str()));
    }
    catch(ProductModel::ERROR_WHILE_LOADING err){
        QString msg{"Duplicate ID found while loading product data at line number of "};
        msg+=QString::number(err.line);
        QMessageBox::critical(this, tr("FAIL LOADING"), tr(msg.toStdString().c_str()));
    }
    catch(OrderModel::ERROR_WHILE_LOADING err){
        QString msg{"Duplicate ID found while loading order data at line number of "};
        msg+=QString::number(err.line);
        QMessageBox::critical(this, tr("FAIL LOADING"), tr(msg.toStdString().c_str()));
    }

    catch(...){
        QMessageBox::critical(this, tr("FAIL LOADING"), tr("FAIL LOADING"));
    }

    mgrs.updateAll();//로드한 후 모든 뷰(탭)를 갱신합니다
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dash_board;
}

void Manager::attachObserver(View* o){
    observers.emplace_back(o);
}
void Manager::detachObserver(View* o){
    observers.remove(o);
}
void Manager::reset(){
    delete cm;
    delete pm;
    delete om;
}
