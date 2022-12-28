#ifndef VIEWFACTORY_H
#define VIEWFACTORY_H

#include <string>

#include <QIcon>

class View;
class MainManager;
class Tree;

class ViewFactory {             //팩토리 패턴을 위한 클래스입니다. QTabWidget의 이름과 아이콘을 멤버로 갖습니다. make 함수를 상속받는 클래스가 정의하도록 하였습니다
protected:
    const std::string title;
    const QIcon icon;
public:
    ViewFactory(const QIcon& icon, const std::string title) : title{title}, icon{icon} {}
    virtual ~ViewFactory()=default;
    virtual View* make(Tree*)=0;                    //ViewFactory 클래스를 상속하는 클래스는 make 함수를 재정의 해야하며 View*를 리턴해야 합니다. 이 View는 QTabWidget에 쓰입니다
    std::string getTitle() const { return title; }
    const QIcon& getIcon() const { return icon; }
};
template<typename T>
class ViewMaker : public ViewFactory{       //addView, ShowView에 있는 클래스를 생성하는 클래스입니다. 팩토리 구현 부
    MainManager& mgr;
public:
    ViewMaker(MainManager& mgr,const QIcon& icon,std::string title) : mgr{mgr}, ViewFactory{icon,title} {}
    ~ViewMaker() override {}
    View* make(Tree* tree) override { return new T{mgr,*tree,icon,title}; }   //addView, showView 모두 같은 인자로 생성가능 합니다
};

#endif // VIEWFACTORY_H
