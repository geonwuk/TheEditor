#include "tree.h"
#include "View/addview.h"
#include "View/showview.h"
#include <QLabel>


Tree::Tree(MainWindow* mw, TabWidget *tw, int tabs_item_position) : QTreeWidget{mw}, mw{mw}, mgr{*mw->getMgr()}, tw{tw}, tabs_item_position{tabs_item_position}
{
    setHeaderHidden(true);
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(_itemDoubleClicked(QTreeWidgetItem*, int)));
    QObject::connect(tw,SIGNAL(tabCurrentChanged_(int)),SLOT(tabCurrnetChanged(int)));
}

void Tree::tabCurrnetChanged(int index){
    if(index==-1)
        return;
    auto tabs = topLevelItem(tabs_item_position);
    auto child = tabs->child(index);
    auto prev_child = tabs->child(prev_tabs_index);

    QFont un_bold(child->font(0));
    un_bold.setBold(false);
    prev_child->setFont(0,un_bold);

    QFont bold(child->font(0));
    bold.setBold(true);
    child->setFont(0,bold);

    prev_tabs_index=index;
}

void Tree::_itemDoubleClicked(QTreeWidgetItem* item, int){
    TreeItem* it = dynamic_cast<TreeItem*>(item);
    it->doubleClicked();
}



View* Tree::makeView(ViewFactory* factory){
    View* view = factory->make(this);
    mgr.attachObserver(view);
    return view;
}

FocusTabItem::FocusTabItem(View* view, Tree& tree, const QIcon& icon, QString title):TreeItem{tree,icon,title},view{view}{

}

void FocusTabItem::doubleClicked(){
    tree.tw->setCurrentWidget(view);
}



void ToTabItem::doubleClicked(){
    View* view = tree.makeView(view_factory);
    tree.tw->addTab(view, view->icon, view->label);
    tree.tw->setCurrentWidget(view);
}


ManagementTree::ManagementTree(MainWindow* mw, TabWidget* tw) : Tree{mw,tw,3} {
    QList<ViewFactory*> add_views { new ViewMaker<AddClientView>{mgr,QIcon(":/Icons/client.png"),tr("Add Client")},new ViewMaker<AddProductView>{mgr,QIcon(":/Icons/product.png"),tr("Add Product")},new ViewMaker<AddOrderView>{mgr,QIcon(":/Icons/order.png"),tr("Add Order")}};
    QList<ViewFactory*> show_views { new ViewMaker<ShowClientView>{mgr,QIcon(":/Icons/client.png"),tr("Show Client")},new ViewMaker<ShowProductView>{mgr,QIcon(":/Icons/product.png"),tr("Show Product")},new ViewMaker<ShowOrderView>{mgr,QIcon(":/Icons/order.png"),tr("Show Order")}};

    tabs = new TreeItem(*this, QPixmap(), {tr("Tabs")});

    QStringList top_ls = {tr("Client Management"), tr("Product Management"), tr("Order Management")};

    int i=0;
    for(auto& e : top_ls) {
        addTopLevelItem(new TreeItem(*this,QPixmap(),e));
        auto elem = topLevelItem(i);
        expandItem(elem);
        auto title = elem->text(0).split(" ");

        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this, add_views[i]->getIcon(), {tr("%1 Add").arg(title[0])} );
        elem->addChild(add);


        QTreeWidgetItem* show = new ToTabItem(show_views[i], *this, show_views[i]->getIcon(), {tr("%1 Show / Find / Erase").arg(title[0])} );
        elem->addChild(show);

        i++;
    }


    addTopLevelItem(tabs);
    expandItem(tabs);
}

NetworkTree::NetworkTree(MainWindow* mw, TabWidget* tw) : Tree{mw,tw,1}{
    QList<ViewFactory*> add_views { new ViewMaker<AddParticipantView>{mgr,QIcon(),tr("Add Paticipants")}};
    QList<ViewFactory*> show_views {new ViewMaker<ShowChatView>{mgr,QIcon(":/Icons/chat.png"),tr("Show Chat")}};
    QStringList top_ls = {tr("Network Management")};

    int i=0;
    for(auto& e : top_ls) {
        addTopLevelItem(new TreeItem(*this,QPixmap(),{e}));
        auto elem = topLevelItem(i);
        expandItem(elem);

        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this,add_views[i]->getIcon(), add_views[i]->getTitle() );
        elem->addChild(add);

        QTreeWidgetItem* show = new ToTabItem(show_views[i], *this,show_views[i]->getIcon(), show_views[i]->getTitle() );
        elem->addChild(show);

        i++;
    }
    tabs = new TreeItem(*this,QPixmap(), {tr("Tabs")});
    addTopLevelItem(tabs);
    expandItem(tabs);

}

Tree::~Tree(){

}
