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
public:
     ViewFactory(const QString title) : title{title} {}
    virtual View* make(Tree* tree)=0;
    const QString getTitle(){
        return title;
    }
};
template<typename T>
class ViewMaker : public ViewFactory{
    Manager& mgr;
public:
    ViewMaker(Manager& mgr,QString title) : mgr{mgr}, ViewFactory{title} {}
    View* make(Tree* tree){
        return new T{mgr,*tree,QPixmap(),title};
    }

};

class Tree : public QTreeWidget {
    Q_OBJECT

public:
    ~Tree();
    Tree(MainWindow* , TabWidget* tw);
    QTreeWidgetItem& getTabs(){return *tabs;}
    View* makeView(ViewFactory* factory);

protected:
    MainWindow* mw;

    Manager& mgr;
    QTreeWidgetItem* tabs;

public:
    TabWidget* tw;

signals:
    void treeToTab(QWidget*, const QIcon&, const QString&);
    void setTabFocus(QWidget* page);

public slots:
    void _itemDoubleClicked(QTreeWidgetItem*, int);

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

public:
    TreeItem(Tree& tree, QString title):QTreeWidgetItem{(QTreeWidget*)nullptr,{title}},tree{tree}{}
    virtual void doubleClicked(){qDebug()<<"default double";};
};

class ToTabItem : public TreeItem{
    ViewFactory* view_factory;
public:
    ToTabItem(ViewFactory* view_factory, Tree& tree, QString title):TreeItem{tree,{title}},view_factory{view_factory}{}
    void doubleClicked();
};

class FocusTabItem : public TreeItem{
    View* view;
public:
    FocusTabItem(View* view, Tree& tree, QString title):TreeItem{tree,{title}},view{view}{}
    void doubleClicked();
    ~FocusTabItem(){}
};


#endif // TREE_H
