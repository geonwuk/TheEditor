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

    tw.setTabsClosable(true);
    QSizePolicy tab_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tab_policy.setHorizontalStretch(100);                                       //화면이 확대될 때 트리는 가만히, 탭 화면만 확대되야 함
    tw.setSizePolicy(tab_policy);                                               //Expanding 사이즈 정책 적용
    splitter->addWidget(&tw);                                                   //스플리터에 추가

    return splitter;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sw=ui->stackedWidget;
    ui->actionSave->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogSaveButton));      //save 액션 아이콘 추가
    ui->actionOpen->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogOpenButton));      //load 액션 아이콘 추가

    QSplitter* splitter = initTreeAndTab(management_tree,management_tw);                     //고객,물품,주문관리 splitter
    QSplitter* splitter2 = initTreeAndTab(network_tree, network_tw);                        //네트워크 splitter

    sw->addWidget(splitter);         //고객,물품,주문관리
    sw->addWidget(splitter2);        //네트워크
    connect(ui->ManagementButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(0);}); //고객,물품,주문관리
    connect(ui->ChatButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(1); });      //네트워크

    mgrs.getSM().setServer(new Server{mgrs.getSM()});                                   //서버 생성 후 포인터로 설정

    connect(ui->actionSave,SIGNAL(triggered()),SLOT(save()));       //저장
    connect(ui->actionOpen,SIGNAL(triggered()),SLOT(load()));       //불러오기
    setWindowTitle(tr("Program"));

//    ui->verticalLayout->sizeHint();
}

void Manager::updateAll(){          //파일을 불러오기한 경우 모두 업데이트 한다
    for (auto o : observers) {
        o->update();
    }
}

void MainWindow::save(){                                                //파일을 CSV포맷으로 저장하는 함수로 QAction과 연결되어 있다
    QString filename = QFileDialog::getSaveFileName(this);
    std::ofstream out(filename.toStdString());
    out<<"[Clients],"<<mgrs.getCM().getSize()<<','<<std::endl;      //고객
    mgrs.getCM().saveClients(out);
    out<<"[Products],"<<mgrs.getPM().getSize()<<','<<std::endl;     //물품
    mgrs.getPM().saveProducts(out);
    out<<"[Orders],"<<mgrs.getOM().getSize()<<','<<std::endl;       //주문
    mgrs.getOM().saveOrders(out);
}
void MainWindow::load(){                                                        //CSV 포맷으로 저장된 파일을 불러오는 함수로 QAction과 연결되어 있다
    try {                                                                       //try-catch를 써서 로딩 중에 프로그램이 죽지 않고 로딩 실패가 되도록 로직코드를 감싸준다
        QString filename = QFileDialog::getOpenFileName(this);
        ifstream in(filename.toStdString());
        unsigned int line=0;
        line = parseTitle(in,"[Clients]",line);
        mgrs.getCM().loadClients(in, line);
        line = parseTitle(in,"[Products]",line);
        mgrs.getPM().loadProducts(in, line);
        line = parseTitle(in,"[Orders]",line);
        mgrs.getOM().loadOrders(in, line);
        mgrs.updateAll();
    } catch (...) {
        QMessageBox::critical(this, tr("FAIL LOADING"), tr("FAIL LOADING"));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void Manager::attachObserver(View* o){
    observers.emplace_back(o);
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


