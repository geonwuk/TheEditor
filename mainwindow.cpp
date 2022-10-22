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
    connect(ui->ManagementButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(1);
    qDebug()<<"1번 누름";});
    connect(ui->ChatButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(2); qDebug()<<"2번 누름"; });

    mgrs.getSM().setServer(new Server{mgrs.getSM()});
    //connect(&tree,SIGNAL(setTabFocus(QWidget*)),SLOT(setTabFocus(QWidget*)));

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
    QString filename = QFileDialog::getOpenFileName(this);
    std::ofstream out(filename.toStdString());
    out<<"[Clients],"<<mgrs.getCM().getSize()<<std::endl;
    mgrs.getCM().saveClients(out);
    out<<"[Products],"<<mgrs.getPM().getSize()<<std::endl;
    mgrs.getPM().saveProducts(out);
    out<<"[Orders],"<<mgrs.getOM().getSize()<<std::endl;
    mgrs.getOM().saveOrders(out);
}
void MainWindow::load(){

}

MainWindow::~MainWindow()
{
    delete ui;
}

void Manager::attachObserver(View* o){
    qDebug()<<"attach observer";
    observers.emplace_back(o);
}




