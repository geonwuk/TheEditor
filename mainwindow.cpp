#include "mainwindow.h"

#include <QLabel>
#include <QList>
#include "ui_mainwindow.h"
#include "Network/server.h"
#include "tree.h"
#include "View/view.h"
#include <QSplitter>
#include "Network/server.h"
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

    Server* s = new Server{mgrs.getSM()};
    //connect(&tree,SIGNAL(setTabFocus(QWidget*)),SLOT(setTabFocus(QWidget*)));
}

void MainWindow::treeToTab(QWidget *page, const QIcon &icon, const QString &label){
//    tw.addTab(page, icon, label);
//    tw.setCurrentWidget(page);
}
void MainWindow::setTabFocus(QWidget* page){
//    tw.setCurrentWidget(page);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void Manager::attachObserver(View* o){
    qDebug()<<"attach observer";
    observers.emplace_back(o);
}


