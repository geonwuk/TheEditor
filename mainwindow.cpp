#include "mainwindow.h"

#include <QLabel>
#include <QList>
#include "ui_mainwindow.h"
#include "tree.h"
#include "View/view.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->splitter->setChildrenCollapsible(false);


    tree.sizePolicy().setHorizontalStretch(0);
    QSizePolicy tree_policy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    tree_policy.setHorizontalStretch(0);
    tree.setSizePolicy(tree_policy);
    tree.setBaseSize(width(),height());
    ui->splitter->addWidget(&tree);


    tw.setTabsClosable(true);
    QSizePolicy tab_policy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    tab_policy.setHorizontalStretch(100);
    tw.setSizePolicy(tab_policy);
    ui->splitter->addWidget(&tw);

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


