#include "mainwindow.h"

#include <QLabel>
#include <QList>
#include "ui_mainwindow.h"
#include "Network/network.h"
#include "tree.h"
#include "View/view.h"
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sw=ui->stackedWidget;



    QWidget* manage_widget = new QWidget;
    QSplitter* splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);


    tree.sizePolicy().setHorizontalStretch(0);
    QSizePolicy tree_policy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    tree_policy.setHorizontalStretch(0);
    tree.setSizePolicy(tree_policy);
    tree.setBaseSize(width(),height());
    splitter->addWidget(&tree);

    tw.setTabsClosable(true);
    QSizePolicy tab_policy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    tab_policy.setHorizontalStretch(100);
    tw.setSizePolicy(tab_policy);
    splitter->addWidget(&tw);


    sw->addWidget(new QWidget);
    sw->addWidget(splitter);
    QWidget* net = new NetWork;
    sw->addWidget(net);
    connect(ui->ManagementButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(1);
    qDebug()<<"1번 누름";});
    connect(ui->ChatButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(2); qDebug()<<"2번 누름"; });

}

void MainWindow::treeToTab(QWidget *page, const QIcon &icon, const QString &label){
    tw.addTab(page, icon, label);
    tw.setCurrentWidget(page);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void Manager::attachObserver(View* o){
    qDebug()<<"attach observer";
    observers.emplace_back(o);
}


