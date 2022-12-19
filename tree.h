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
class ViewFactory {             //팩토리 패턴을 위한 클래스입니다. QTabWidget의 이름과 아이콘을 멤버로 갖습니다. make 함수를 상속받는 클래스가 정의하도록 하였습니다
protected:
    const QString title;
    const QIcon icon;
public:
    ViewFactory(const QIcon& icon, const QString title) : title{title}, icon{icon} {}
    virtual ~ViewFactory(){}
    virtual View* make(Tree*)=0;                    //ViewFactory 클래스를 상속하는 클래스는 make 함수를 재정의 해야하며 View*를 리턴해야 합니다. 이 View는 QTabWidget에 쓰입니다
    QString getTitle() const { return title; }
    const QIcon& getIcon() const { return icon; }
};
template<typename T>
class ViewMaker : public ViewFactory{       //addView, ShowView에 있는 클래스를 생성하는 클래스입니다. 팩토리 구현 부
    Manager& mgr;
public:
    ViewMaker(Manager& mgr,const QIcon& icon,QString title) : mgr{mgr}, ViewFactory{icon,title} {}
    ~ViewMaker() override {}
    View* make(Tree* tree) override { return new T{mgr,*tree,icon,title}; }   //addView, showView 모두 같은 인자로 생성가능 합니다
};
class Tree : public QTreeWidget {
    Q_OBJECT
    friend class ToTabItem;
    friend class FocusTabItem;
public:
    ~Tree();
    Tree(MainWindow* , TabWidget* tw, int tabs_item_position);
    QTreeWidgetItem& getTabs() const {return *tabs;}
    View* makeView(ViewFactory* factory);
protected:
    MainWindow* mw;
    void tabCurrnetChanged(int index, int top_level_item_position);
    Manager& mgr;
    QTreeWidgetItem* tabs;      //트리에서 가장 밑에 있는 ToplevelItem
    TabWidget* tw;
    int prev_tabs_index=0;      //트리에서 가장 밑에 있는 ToplevleItem의 자식 중에서 현재 선택된
    int tabs_item_position;     //
//signals:
//    void treeToTab(QWidget*, const QIcon&, const QString&);
//    void setTabFocus(QWidget* page);
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
    TreeItem(Tree& tree, const QIcon icon, QString title):QTreeWidgetItem{(QTreeWidget*)nullptr,{title}},tree{tree}{
        setIcon(0,icon);
    }
    virtual void doubleClicked(){};
};
class ToTabItem : public TreeItem{          //이 TreeItem을 클릭하면 새로운 QTabWidgetItem을 생성합니다. (ViewFactory*를 생성자로 갖는 이유)
    ViewFactory* view_factory;
public:
    ToTabItem(ViewFactory* view_factory, Tree& tree, const QIcon icon, QString title):TreeItem{tree,icon,title},view_factory{view_factory}{}
    ~ToTabItem(){delete view_factory;}
    void doubleClicked();
};
class FocusTabItem : public TreeItem{       //QTreeWidgetItem을 상속하며 이 treeWidgetItem을 클릭하면 열려진 Tab 중을 현재 위젯으로 설정합니다
    View* view; //View는 QTabWidget에서 지웁니다
public:
    FocusTabItem(View* view, Tree& tree, const QIcon icon, QString title):TreeItem{tree,icon,title},view{view}{}
    void doubleClicked();
    ~FocusTabItem(){}
};
#endif // TREE_H
