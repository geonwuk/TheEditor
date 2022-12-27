#ifndef TREE_H
#define TREE_H

#include <QTreeWidget>

class ViewFactory;
class MainWindow;
class View;
class MainManager;
class TabWidget;

class Tree : public QTreeWidget {
    Q_OBJECT
    friend class ToTabItem;
    friend class FocusTabItem;
public:
    Tree(MainWindow* , TabWidget* tw, int tabs_item_position);
    ~Tree();
    QTreeWidgetItem& getTabs() const {return *tabs;}
    View* makeView(ViewFactory* factory);
protected:
    MainWindow* m_main_window;             //Main이 삭제
    void tabCurrnetChanged(int index, int top_level_item_position);
    MainManager& m_manager;
    QTreeWidgetItem* tabs;      //소멸자에서 삭제  (트리에서 가장 밑에 있는 ToplevelItem)
    TabWidget* m_tab_window;              //MainWindow에서 삭제함
    int prev_tabs_index;        //트리에서 가장 밑에 있는 ToplevleItem의 자식 중에서 현재 선택된
    int tabs_item_position;
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
    std::string title;
    QIcon icon;
public:
    TreeItem(Tree& tree, const QIcon icon, std::string title):QTreeWidgetItem{(QTreeWidget*)nullptr,{QString::fromStdString(title)}},tree{tree}{
        setIcon(0,icon);
    }
    virtual void doubleClicked(){};
};


class ToTabItem : public TreeItem{          //이 TreeItem을 클릭하면 새로운 QTabWidgetItem을 생성합니다. (ViewFactory*를 생성자로 갖는 이유)
    ViewFactory* view_factory;      //소멸자에서 삭제
public:
    ToTabItem(ViewFactory* view_factory, Tree& tree, const QIcon icon, std::string title):TreeItem{tree,icon,title},view_factory{view_factory}{}
    ~ToTabItem(){delete view_factory; view_factory = nullptr;}
    void doubleClicked();
};


class FocusTabItem : public TreeItem{       //QTreeWidgetItem을 상속하며 이 treeWidgetItem을 클릭하면 열려진 Tab 중을 현재 위젯으로 설정합니다
    View* view; //View는 TabWidget에서 삭제
public:
    FocusTabItem(View* view, Tree& tree, const QIcon icon, std::string title):TreeItem{tree,icon,title},view{view}{}
    void doubleClicked();
    ~FocusTabItem(){}
};
#endif // TREE_H
