#include "tabwidget.h"
#include "View/view.h"

void TabWidget::tabClosed(int index){
    View* wg = static_cast<View*>(widget(index));
    removeTab(index);
    wg->removeFromTree();
    delete wg;
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

TabWidget::~TabWidget(){

}
