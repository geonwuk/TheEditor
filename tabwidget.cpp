#include "tabwidget.h"

#include <cassert>

#include "View/view.h"

void TabWidget::tabClosed(int index){
    blockSignals(true);
    View* wg = static_cast<View*>(widget(index));   //탭 위젯의 모든 뷰는 View클래스를 상속한다
    removeTab(index);       //QTabWidget의 removeTab 호출
    wg->removeFromTree();   //지우기 전에 트리에서 먼저 목록을 삭제한다
    delete wg;
    blockSignals(false);
    emit tabCurrentChanged_(currentIndex());
}

void TabWidget::tabCurrentChanged(int index){
    emit tabCurrentChanged_(index);             //탭이바뀌면 Tree의 Tabs항목의 굵은 글씨도 바껴야 하므로 시그널을 송출
}

TabWidget::TabWidget(MainWindow* parent) : parent(parent) {
    assert(connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosed(int))));
    assert(connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabCurrentChanged(int))));
}
