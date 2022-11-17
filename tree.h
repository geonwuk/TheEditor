#ifndef TREE_H
#define TREE_H
#include <QTreeWidget>
#include <vector>
#include <functional>



class QTreeWidgetItem;
class QVariant;
class MainWindow;
class View;
class Manager;
class Tree;
class TabWidget;
class ViewFactory {
protected:
    const QString title;
    const QIcon icon;
public:
    ViewFactory(const QIcon& icon, const QString title) : title{title}, icon{icon} {}
    virtual View* make(Tree*)=0;
    const QString getTitle(){
        return title;
    }
    const QIcon& getIcon(){
        return icon;
    }
};
template<typename T>
class ViewMaker : public ViewFactory{
    Manager& mgr;
public:
    ViewMaker(Manager& mgr,const QIcon& icon,QString title) : mgr{mgr}, ViewFactory{icon,title} {}
    View* make(Tree* tree){
        return new T{mgr,*tree,icon,title};
    }

};

class Tree : public QTreeWidget {
    Q_OBJECT
    friend class ToTabItem;
    friend class FocusTabItem;
public:
    ~Tree();
    Tree(MainWindow* , TabWidget* tw, int tabs_item_position);
    QTreeWidgetItem& getTabs(){return *tabs;}
    View* makeView(ViewFactory* factory);

protected:
    MainWindow* mw;
    void tabCurrnetChanged(int index, int top_level_item_position);
    Manager& mgr;
    QTreeWidgetItem* tabs;
    TabWidget* tw;
    int prev_tabs_index=0;
    int tabs_item_position;

signals:
    void treeToTab(QWidget*, const QIcon&, const QString&);
    void setTabFocus(QWidget* page);

public slots:
    void _itemDoubleClicked(QTreeWidgetItem*, int);
    void tabCurrnetChanged(int index);


};

class ManagementTree : public Tree{
public:
    ManagementTree(MainWindow*, TabWidget* tw);
};
class NetworkTree : public Tree{
public:
    NetworkTree(MainWindow*, TabWidget* tw);
};


class TreeItem : public QTreeWidgetItem{
protected:
    Tree& tree;
    QString title;
    QIcon icon;
public:
    TreeItem(Tree& tree, const QIcon& icon, QString title):QTreeWidgetItem{(QTreeWidget*)nullptr,{title}},tree{tree}{
        setIcon(0,icon);
    }
    virtual void doubleClicked(){};
};

class ToTabItem : public TreeItem{
    ViewFactory* view_factory;
public:
    ToTabItem(ViewFactory* view_factory, Tree& tree, const QIcon& icon, QString title):TreeItem{tree,icon,title},view_factory{view_factory}{}
    void doubleClicked();
};

class FocusTabItem : public TreeItem{
    View* view;
public:
    FocusTabItem(View* view, Tree& tree, const QIcon& icon, QString title);
    void doubleClicked();
    ~FocusTabItem(){}
};


#endif // TREE_H
