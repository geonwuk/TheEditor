#include "Tree.h"

#include <cassert>
#include <QLabel>

#include "View/AddView.h"
#include "View/showview.h"
#include "View/ViewFactory.h"

#include "mainwindow.h"

Tree::Tree(MainWindow* mw, TabWidget *tab_window, int tabs_item_position) : QTreeWidget{mw}, m_main_window{mw}, m_manager{*mw->getMgr()}, tabs{nullptr}, m_tab_window{tab_window}, prev_tabs_index{0},  tabs_item_position{tabs_item_position}
{
    setHeaderHidden(true);
    assert(connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(_itemDoubleClicked(QTreeWidgetItem*,int))));
    assert(connect(m_tab_window, SIGNAL(tabCurrentChanged_(int)), SLOT(tabCurrnetChanged(int))));
}

Tree::~Tree(){
    assert(disconnect(this));
//    assert(disconnect(tw)); -> TabWidget이 disconnect 해야함
    delete tabs;
    tabs=nullptr;
}

void Tree::tabCurrnetChanged(int index){
    //활성화된 아이템의 텍스트를 볼드처리함으로써$ 무슨 탭이 활성화 됐는지 알려주는 기능을 구현합니다
    if(index==-1)                                       //탭이 하나도 없을 경우 리턴
        return;
    auto tabs = topLevelItem(tabs_item_position);
    auto child = tabs->child(index);                    //볼드 설정할 아이템
    auto prev_child = tabs->child(prev_tabs_index);     //볼드 해제할 아이템

    QFont un_bold(child->font(0));
    un_bold.setBold(false);
    if(prev_child!=nullptr)
    prev_child->setFont(0,un_bold);      //이전에 활성화된 아이템의 텍스트의 볼드를 해제합니다

    QFont bold(child->font(0));
    bold.setBold(true);
    child->setFont(0,bold);             //새로 활성화 된 아이템의 텍스트를 볼드로 바꿉니다

    prev_tabs_index=index;              //재사용을 위해 새롭게 선택된 아이템이 나중에는 이전 아이템으로 됩니다
}

void Tree::_itemDoubleClicked(QTreeWidgetItem* item, int){
    TreeItem* it = dynamic_cast<TreeItem*>(item);           //ToTabItem 또는 FocusTabItem 둘 주에 하나
    it->doubleClicked();
}

View* Tree::makeView(ViewFactory* factory){
    View* view = factory->make(this);                       //make: 팩토리 패턴의 생성자 추상화
    m_manager.attachObserver(view);                               //옵저버 패턴을 위해 생성 후 관리자에게 등록한다
    return view;                                            //
}

void FocusTabItem::doubleClicked(){     //더블 클릭하면 QTabWidget에서 열려진 기존의 Tab을 현재 위젯으로 설정합니다
    tree.m_tab_window->setCurrentWidget(view);
}

void ToTabItem::doubleClicked(){
    View* view = tree.makeView(view_factory);           //상태 패턴+팩토리 패턴 (view는 TabWidget에서 삭제)
    tree.m_tab_window->addTab(view, view->icon, view->label.c_str());     //tree.tw <- QTabWidget을 상속받는 탭 위젯으로 탭에 추가
    tree.m_tab_window->setCurrentWidget(view);
}

ManagementTree::ManagementTree(MainWindow* mw, TabWidget* tw) : Tree{mw,tw,3} {
    QList<ViewFactory*> add_views { new ViewMaker<AddClientView>{m_manager,QIcon(":/Icons/client.png"),tr("Add Client").toStdString()},new ViewMaker<AddProductView>{m_manager,QIcon(":/Icons/product.png"),tr("Add Product").toStdString()},new ViewMaker<AddOrderView>{m_manager,QIcon(":/Icons/order.png"),tr("Add Order").toStdString()}};
    QList<ViewFactory*> show_views { new ViewMaker<ShowClientView>{m_manager,QIcon(":/Icons/client.png"),tr("Show Client").toStdString()},new ViewMaker<ShowProductView>{m_manager,QIcon(":/Icons/product.png"),tr("Show Product").toStdString()},new ViewMaker<ShowOrderView>{m_manager,QIcon(":/Icons/order.png"),tr("Show Order").toStdString()}};
    //add_view: 데이터 추가하는 View 클래스 모음
    //show_view: 데이터를 보여주고 수정하는 View 클래스 모음

    tabs = new TreeItem(*this, QPixmap(), {tr("Tabs").toStdString()});    //트리에서 Tabs의 자식은 FocusTabItem이다

    QStringList top_ls = {tr("Client Management"), tr("Product Management"), tr("Order Management")};

    int i=0;
    for(auto& e : top_ls) {
        addTopLevelItem(new TreeItem(*this,QPixmap(),e.toStdString()));
        auto elem = topLevelItem(i);
        expandItem(elem);                       //트리 펼치기
        auto title = elem->text(0).split(" ");  //title[0]에는 Cleint, Product, Order 셋 중에 하나

        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this, add_views[i]->getIcon(), {tr("%1 Add").arg(title[0]).toStdString()} );   //ToTabItem: 더블클릭하면 QTabWidget에 View를 추가합니다
        elem->addChild(add);

        QTreeWidgetItem* show = new ToTabItem(show_views[i], *this, show_views[i]->getIcon(), {tr("%1 Show / Find / Erase").arg(title[0]).toStdString()} );   //ToTabItem: 더블클릭하면 QTabWidget에 View를 추가합니다
        elem->addChild(show);

        i++;
    }

    addTopLevelItem(tabs);
    expandItem(tabs);
}

NetworkTree::NetworkTree(MainWindow* mw, TabWidget* tw) : Tree{mw,tw,1}{
    QList<ViewFactory*> add_views { new ViewMaker<AddParticipantView>{m_manager,QIcon(),tr("Add Paticipants").toStdString()}};
    QList<ViewFactory*> show_views {new ViewMaker<ShowChatView>{m_manager,QIcon(":/Icons/chat.png"),tr("Show Chat").toStdString()}};
    QStringList top_ls = {tr("Network Management")};

    int i=0;
    for(const auto& str : top_ls) {
        addTopLevelItem(new TreeItem(*this,QPixmap(),{str.toStdString()}));
        auto elem = topLevelItem(i);
        expandItem(elem);

        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this,add_views[i]->getIcon(), add_views[i]->getTitle() );
        elem->addChild(add);

        QTreeWidgetItem* show = new ToTabItem(show_views[i], *this,show_views[i]->getIcon(), show_views[i]->getTitle() );
        elem->addChild(show);

        i++;
    }
    tabs = new TreeItem(*this,QPixmap(), {tr("Tabs").toStdString()}); //트리에서 Tabs의 자식은 FocusTabItem이다
    addTopLevelItem(tabs);
    expandItem(tabs);
}


