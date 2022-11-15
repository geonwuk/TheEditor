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
using namespace std;
static const unsigned int parseTitle(ifstream& in, const string title, const unsigned int line );
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
    connect(dash_board->radioButtonMemory, SIGNAL(clicked()), this, SLOT(onRadioButtonMemoryClicked()));


}


void Manager::updateAll(){          //파일을 불러오기한 경우 모두 업데이트 한다
    for (auto o : observers) {
        o->update();
    }
}

void MainWindow::onRadioButtonDBClicked(){


}
void MainWindow::onRadioButtonMemoryClicked(){

}

static int createListDialog(QWidget* parent, QStringList ls){
    QDialog* dialog = new QDialog(parent);
    dialog->setModal(true);
    QListWidget* list = new QListWidget(dialog);
    for(auto e : ls){
        list->addItem(e);
    }
    QPushButton* ok= new QPushButton("OK",dialog);
    QPushButton* cancel = new QPushButton("cancel",dialog);
    QVBoxLayout* v_layout = new QVBoxLayout;
    v_layout->addWidget(list);
    v_layout->addWidget(ok);
    v_layout->addWidget(cancel);
    dialog->setLayout(v_layout);
    QObject::connect(ok,SIGNAL(clicked()),dialog,SLOT(accept()));
    QObject::connect(cancel,SIGNAL(clicked()),dialog,SLOT(rejected()));
    dialog->exec();
    int result = list->currentIndex().row();
    delete dialog;
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
                save_product.addProduct(p.getId(),p.getName(),p.getPrice(),p.getQty(),p.getDate());
            }
            DBM::OrderManager save_order{mgrs.getCM(),mgrs.getPM(),"save_order",file_name};
            for(const auto& o : mgrs.getOM()){
                std::vector<OrderModel::bill> bills;
                for(const auto& ordered_product : o.getProducts()){
                    bills.emplace_back(ordered_product.product.getId(),ordered_product.qty);
                }
                save_order.addOrder(o.getClient().getId(),bills);
            }
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
                out<<"order id,"<<"buyer,"<<"date,"<<"product id,"<<"product name,"<<"product qty"<<"product date,"<<endl;
                for (const auto& o : mgrs.getOM()){
                    out << o <<',' << endl;
                }
                break;
            default://있을 수 없는 경우
                assert(false);
                break;
            };
        }
        catch(...){

        }

    }




}
void MainWindow::load(){
    //저장된 파일을 불러오는 함수로 QAction과 연결되어 있다
    QString file_name = QFileDialog::getSaveFileName(this);
    if(file_name.size()==0){
        return;
    }
    int mode = createListDialog(this,{"DB","CSV"});

    switch(mode){
    case 0://DB로부터 데이터 로드
        mgrs.getCM().loadClients(file_name);
        break;
    case 1://CSV로부터 데이터 로드

    default:
        assert(false);
        break;
    }


    try {                                                                       //try-catch를 써서 로딩 중에 프로그램이 죽지 않고 로딩 실패가 되도록 로직코드를 감싸준다
        QString filename = QFileDialog::getOpenFileName(this);
        ifstream in(filename.toStdString());
        unsigned int line=0;
        line = parseTitle(in,"[Clients]",line);
        //mgrs.getCM().loadClients(in, line);
        line = parseTitle(in,"[Products]",line);
        //mgrs.getPM().loadProducts(in, line);
        line = parseTitle(in,"[Orders]",line);
        //mgrs.getOM().loadOrders(in, line);
        mgrs.updateAll();
    } catch (...) {
        QMessageBox::critical(this, tr("FAIL LOADING"), tr("FAIL LOADING"));
    }
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
    if(mw.dash_board->radioButtonDB->isChecked()){
        changeToDB();
    }
    else{
        changeToMemory();
    }
}

void Manager::changeToDB(){
    delete cm;
    delete pm;
    delete om;
    cm = new DBM::ClientManager{"client"};
    pm = new DBM::ProductManager{"product"};
    om = new DBM::OrderManager{*cm,*pm,"orders"};
}
void Manager::changeToMemory(){
    delete cm;
    delete pm;
    delete om;
    cm = new CM::ClientManager;
    pm = new PM::ProductManager;
    om = new OM::OrderManager{*cm,*pm};
}

static const unsigned int parseTitle(ifstream& in, const string title, const unsigned int line ){
    //[Clients],[Products],[Orders] 옆에는 몇개의 데이터가 있는지 표시하도록 되어 있으므로
    //몇개의 데이터를 불러오기 해야하는지 추출하는 함수입니다
    try{
        string str;
        if(!getline(in, str, ',')){     //파일에서 [Clients] 또는 [Products] 또는 [Orders] 스트링 데이터를 추출합니다
            throw line;
        }
        if(str!=title){                 //만약 저장된 스트링이 [Clients],[Products],[Orders]가 아니라면 원하는 포맷의 파일이 아니므로 예외처리 합니다
            throw line;
        }
        getline(in, str, ',');          //[Clients],[Products],[Orders] 다음에는 불러와야 할 데이터 수가 저장되어 있으므로 그 수를 추출합니다.
        string temp;
        getline(in,temp);               //라인에서 필요없는 , 를 추출합니다.
        return stoul(str);              //가져와야할 데이터 수를 리턴해서 그만큼 데이터를 불러오도록 할 예정입니다
    }
    catch(...){
        throw;                          //예외가 발생하면 rethrow
    }
}


