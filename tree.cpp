#include "tree.h"
#include "View/addview.h"
#include "View/showview.h"
#include <QLabel>


Tree::Tree(MainWindow* mw) : QTreeWidget{mw}, mw{mw}, mgr{*mw->getMgr()}
{
    QList<ViewFactory*> add_views { new ViewMaker<AddClientView>{mgr},new ViewMaker<AddProductView>{mgr},new ViewMaker<AddOrderView>{mgr}};
    QList<ViewFactory*> show_views { new ViewMaker<ShowClientView>{mgr},new ViewMaker<ShowProductView>{mgr},new ViewMaker<ShowOrderView>{mgr}};


    QStringList top_ls = {tr("Client Management"), tr("Product Management"), tr("Order Management")};

    int i=0;
    for(auto& e : top_ls) {
        addTopLevelItem(new QTreeWidgetItem({e}));
        auto elem = topLevelItem(i);
        expandItem(elem);
        auto title = elem->text(0).split(" ");

        QTreeWidgetItem* add = new QTreeWidgetItem( {tr("%1 Add").arg(title[0])} );
        setChildData(add, add_views[i]);
        elem->addChild(add);


        elem->addChild(new QTreeWidgetItem{ {tr("%1 Erase").arg(title[0])} });
        elem->addChild(new QTreeWidgetItem{ {tr("%1 Find").arg(title[0])} });

        QTreeWidgetItem* show = new QTreeWidgetItem( {tr("%1 show").arg(title[0])} );
        setChildData(show, show_views[i]);
        elem->addChild(show);

        i++;
    }
    connect(this, SIGNAL(treeToTab(QWidget*, const QIcon&, const QString&)), mw, SLOT(treeToTab(QWidget*, const QIcon&, const QString&)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(_itemDoubleClicked(QTreeWidgetItem*, int)));
}



void Tree::_itemDoubleClicked(QTreeWidgetItem* item, int){
    QVariant v = item->data(0,Qt::UserRole);
    ViewFactory* factory = v.value<ViewFactory*>();
    View* view = makeView(factory);
    emit treeToTab(view, view->icon, view->label);
}

template<typename T>
void Tree::setChildData(QTreeWidgetItem* item,  T* view) {
    QVariant data = QVariant::fromValue(view);
    item->setData(0, Qt::UserRole, data);
}


View* Tree::makeView(ViewFactory* factory){
    View* view = factory->make();
    mgr.attachObserver(view);
    return view;
}
//QModelIndex parent(const QModelIndex &child) const;
//QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
//int rowCount(const QModelIndex &parent = QModelIndex()) const;
//int columnCount(const QModelIndex &parent = QModelIndex()) const;
//bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
//QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
