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

class Tree : public QTreeWidget {
    Q_OBJECT

public:
    Tree(MainWindow*);



private:
    MainWindow* mw;
    Manager& mgr;

    class ViewFactory {
    public:
        virtual View* make()=0;
    };
    template<typename T>
    class ViewMaker : public ViewFactory{
        Manager& mgr;
    public:
        ViewMaker(Manager& mgr) : mgr{mgr} {}
        View* make(){
            return new T{mgr};
        }
    };
    template<typename T>
    void setChildData(QTreeWidgetItem*, T* view);

    View* makeView(ViewFactory* factory);

signals:
    void treeToTab(QWidget*, const QIcon&, const QString&);

public slots:
    void _itemDoubleClicked(QTreeWidgetItem*, int);



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
