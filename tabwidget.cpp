#include "tabwidget.h"


void TabWidget::tabClosed(int index){
    QWidget* wg = widget(index);
    removeTab(index);
    //delete wg;
    qDebug()<<"tab "<<index<<" deleted";

}

void TabWidget::tabCurrentChanged(int index){
    QWidget* widget_ = widget(index);
}


void TabWidget::tabInserted(int index){

}

TabWidget::TabWidget(MainWindow* parent) : parent(parent) {
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosed(int)));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabCurrentChanged(int)));
}

