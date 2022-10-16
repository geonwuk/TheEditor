#include "mainwindow.h"

#include <QLabel>
#include <QList>
#include "ui_mainwindow.h"
#include "Network/server.h"
#include "tree.h"
#include "View/view.h"
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sw=ui->stackedWidget;

    QSplitter* splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);

    tree.sizePolicy().setHorizontalStretch(0);
    QSizePolicy tree_policy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    tree_policy.setHorizontalStretch(0);
    tree.setSizePolicy(tree_policy);
    tree.setMaximumSize(1920,1920);
    tree.setBaseSize(width(),height());
    splitter->addWidget(&tree);

    tw.setTabsClosable(true);
    QSizePolicy tab_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tab_policy.setHorizontalStretch(100);
    tw.setSizePolicy(tab_policy);
    splitter->addWidget(&tw);


    sw->addWidget(new QWidget);
    sw->addWidget(splitter);
    QWidget* net = new Server;
    sw->addWidget(net);
    connect(ui->ManagementButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(1);
    qDebug()<<"1번 누름";});
    connect(ui->ChatButton, &QToolButton::pressed,[=]{ sw->setCurrentIndex(2); qDebug()<<"2번 누름"; });

    connect(&tree,SIGNAL(setTabFocus(QWidget*)),SLOT(setTabFocus(QWidget*)));
}

void MainWindow::treeToTab(QWidget *page, const QIcon &icon, const QString &label){
    tw.addTab(page, icon, label);
    tw.setCurrentWidget(page);

}
void MainWindow::setTabFocus(QWidget* page){
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


