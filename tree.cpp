#include "tree.h"
#include "View/addview.h"
#include "View/showview.h"
#include <QLabel>


Tree::Tree(MainWindow* mw, TabWidget *tw) : QTreeWidget{mw}, mw{mw}, mgr{*mw->getMgr()}, tw{tw}
{
    setHeaderHidden(true);
//    QList<ViewFactory*> add_views { new ViewMaker<AddClientView>{mgr,tr("Add Client")},new ViewMaker<AddProductView>{mgr,tr("Add Product")},new ViewMaker<AddOrderView>{mgr,tr("Add Order")}};
//    QList<ViewFactory*> show_views { new ViewMaker<ShowClientView>{mgr,tr("Show Client")},new ViewMaker<ShowProductView>{mgr,tr("Show Product")},new ViewMaker<ShowOrderView>{mgr,tr("Show Order")}};

//    tabs = new TreeItem(*this, {tr("Tabs")});

//    QStringList top_ls = {tr("Client Management"), tr("Product Management"), tr("Order Management")};

//    int i=0;
//    for(auto& e : top_ls) {
//        addTopLevelItem(new TreeItem(*this,{e}));
//        auto elem = topLevelItem(i);
//        expandItem(elem);
//        auto title = elem->text(0).split(" ");

//        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this, {tr("%1 Add").arg(title[0])} );
//        elem->addChild(add);


//        QTreeWidgetItem* show = new ToTabItem(show_views[i], *this, {tr("%1 Show / Find / Erase").arg(title[0])} );
//        elem->addChild(show);

//        i++;
//    }
//    addTopLevelItem(tabs);
//    expandItem(tabs);
//    connect(this, SIGNAL(treeToTab(QWidget*, const QIcon&, const QString&)), mw, SLOT(treeToTab(QWidget*, const QIcon&, const QString&)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(_itemDoubleClicked(QTreeWidgetItem*, int)));
}



void Tree::_itemDoubleClicked(QTreeWidgetItem* item, int){
    TreeItem* it = dynamic_cast<TreeItem*>(item);
    qDebug()<<"double";
    it->doubleClicked();
}



View* Tree::makeView(ViewFactory* factory){
    View* view = factory->make(this);
    mgr.attachObserver(view);
    return view;
}

void FocusTabItem::doubleClicked(){
    qDebug()<<"focus";
    //emit tree.setTabFocus(view);
    tree.tw->setCurrentWidget(view);
}

void ToTabItem::doubleClicked(){
    qDebug()<<"Totab";
    View* view = tree.makeView(view_factory);
    //emit tree.treeToTab(view, view->icon, view->label);
    tree.tw->addTab(view, view->icon, view->label);
    tree.tw->setCurrentWidget(view);
}


ManagementTree::ManagementTree(MainWindow* mw, TabWidget* tw) : Tree{mw,tw} {
    QList<ViewFactory*> add_views { new ViewMaker<AddClientView>{mgr,tr("Add Client")},new ViewMaker<AddProductView>{mgr,tr("Add Product")},new ViewMaker<AddOrderView>{mgr,tr("Add Order")}};
    QList<ViewFactory*> show_views { new ViewMaker<ShowClientView>{mgr,tr("Show Client")},new ViewMaker<ShowProductView>{mgr,tr("Show Product")},new ViewMaker<ShowOrderView>{mgr,tr("Show Order")}};

    tabs = new TreeItem(*this, {tr("Tabs")});

    QStringList top_ls = {tr("Client Management"), tr("Product Management"), tr("Order Management")};

    int i=0;
    for(auto& e : top_ls) {
        addTopLevelItem(new TreeItem(*this,{e}));
        auto elem = topLevelItem(i);
        expandItem(elem);
        auto title = elem->text(0).split(" ");

        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this, {tr("%1 Add").arg(title[0])} );
        elem->addChild(add);


        QTreeWidgetItem* show = new ToTabItem(show_views[i], *this, {tr("%1 Show / Find / Erase").arg(title[0])} );
        elem->addChild(show);

        i++;
    }


    addTopLevelItem(tabs);
    expandItem(tabs);
}

NetworkTree::NetworkTree(MainWindow* mw, TabWidget* tw) : Tree{mw,tw}{
    QList<ViewFactory*> add_views { new ViewMaker<AddParticipantView>{mgr,tr("Add Paticipants")}};
    tabs = new TreeItem(*this, {tr("Tabs")});
    QStringList top_ls = {tr("Network Management")};

    int i=0;
    for(auto& e : top_ls) {
        addTopLevelItem(new TreeItem(*this,{e}));
        auto elem = topLevelItem(i);
        expandItem(elem);

        QTreeWidgetItem* add = new ToTabItem(add_views[i],*this, add_views[i]->getTitle() );
        elem->addChild(add);

        i++;
    }

}
