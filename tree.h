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

class ViewFactory {
public:
    virtual View* make(Tree* tree)=0;
};
template<typename T>
class ViewMaker : public ViewFactory{
    Manager& mgr;
    QString title;
public:
    ViewMaker(Manager& mgr,QString title) : mgr{mgr}, title{title} {}
    View* make(Tree* tree){
        return new T{mgr,*tree,QPixmap(),title};
    }
};

class Tree : public QTreeWidget {
    Q_OBJECT


public:
    Tree(MainWindow*);
    QTreeWidgetItem& getTabs(){return *tabs;}
    View* makeView(ViewFactory* factory);

private:
    MainWindow* mw;
    Manager& mgr;
    QTreeWidgetItem* tabs;





signals:
    void treeToTab(QWidget*, const QIcon&, const QString&);
    void setTabFocus(QWidget* page);

public slots:
    void _itemDoubleClicked(QTreeWidgetItem*, int);



};


template<typename T>
void setChildData(QTreeWidgetItem*, T* view);



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

//using FP = decltype(std::mem_fn<void(),Tree>(nullptr));
//using PP = void (Tree::*)();

//class CallMW
//{
//public:
//    virtual void call()=0;
//};
//struct Con : public CallMW{
//    Tree* tree;
//    FP f;

//    Con(Tree* tree, PP f):tree{tree},f{f} {}
//    void call(){
//        emit f(tree);
//    }

//};



#endif // TREE_H
