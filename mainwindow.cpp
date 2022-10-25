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

using namespace std;
static const unsigned int parseTitle(ifstream& in, const string title, const unsigned int line );
static QSplitter* initTreeAndTab(Tree& tree, TabWidget& tw){
    QSplitter* splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);

    QSizePolicy tree_policy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    tree_policy.setHorizontalStretch(0);
    tree.setSizePolicy(tree_policy);
    tree.setMaximumSize(1920,1920);
    splitter->addWidget(&tree);

    tw.setTabsClosable(true);
    QSizePolicy tab_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tab_policy.setHorizontalStretch(100);
    tw.setSizePolicy(tab_policy);
    splitter->addWidget(&tw);

    return splitter;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sw=ui->stackedWidget;
    ui->actionSave->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionOpen->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogOpenButton));

    QSplitter* splitter = initTreeAndTab(management_tree,management_tw);
    //management_tree.setBaseSize(width(),height());

    QSplitter* splitter2 = initTreeAndTab(network_tree, network_tw);

    sw->addWidget(new QWidget);
    sw->addWidget(splitter);
    //QWidget* net = new Server{mgrs};
    sw->addWidget(splitter2);
    connect(ui->ManagementButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(1);});
    connect(ui->ChatButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(2); });

    mgrs.getSM().setServer(new Server{mgrs.getSM()});
    //connect(&tree,SIGNAL(setTabFocus(QWidget*)),SLOT(setTabFocus(QWidget*)));

    connect(ui->actionSave,SIGNAL(triggered()),SLOT(save()));
    connect(ui->actionOpen,SIGNAL(triggered()),SLOT(load()));
    setWindowTitle(tr("Program"));
}

void MainWindow::treeToTab(QWidget *page, const QIcon &icon, const QString &label){
//    tw.addTab(page, icon, label);
//    tw.setCurrentWidget(page);
}
void MainWindow::setTabFocus(QWidget* page){
//    tw.setCurrentWidget(page);
}

void MainWindow::save(){
    QString filename = QFileDialog::getSaveFileName(this);
    std::ofstream out(filename.toStdString());
    out<<"[Clients],"<<mgrs.getCM().getSize()<<std::endl;
    mgrs.getCM().saveClients(out);
    out<<"[Products],"<<mgrs.getPM().getSize()<<std::endl;
    mgrs.getPM().saveProducts(out);
    out<<"[Orders],"<<mgrs.getOM().getSize()<<std::endl;
    mgrs.getOM().saveOrders(out);
}
void MainWindow::load(){
    try {
        QString filename = QFileDialog::getOpenFileName(this);
        ifstream in(filename.toStdString());
        unsigned int line=0;
        line = parseTitle(in,"[Clients]",line);
        mgrs.getCM().loadClients(in, line);
        line = parseTitle(in,"[Products]",line);
        mgrs.getPM().loadProducts(in, line);
        line = parseTitle(in,"[Orders]",line);
        mgrs.getOM().loadOrders(in, line);
    } catch (...) {
        qDebug()<<"FAIL LOAD";
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void Manager::attachObserver(View* o){
    qDebug()<<"attach observer";
    observers.emplace_back(o);
}


static const unsigned int parseTitle(ifstream& in, const string title, const unsigned int line ){
    try{
        string str;
        if(!getline(in, str, ',')){
            throw line;
        }
        qDebug()<<str.c_str();
        if(str!=title){
            throw line;
        }
        getline(in, str);
        qDebug()<<str.c_str();
        return stoul(str);
    }
    catch(...){
        throw;
    }
}


