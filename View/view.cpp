#include "view.h"
#include "mainwindow.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <iomanip>
#include <sstream>
#include <QDateTimeEdit>
//View::View() {}
View::View(Manager& mgr, Tree& tree, const QIcon &icon, const QString label) : icon{icon}, label{label}, mgr{mgr},tree{tree} {
    tab = new FocusTabItem{this,tree,icon,label};
    tree.getTabs().addChild(tab);
}
View::~View() {
    mgr.detachObserver(this);           //옵저버 패턴: 소멸 시 리스트에서 뺸다
}
void View::removeFromTree(){
    tree.getTabs().removeChild(tab);   //트리에서 View 삭제
}


QWidget* View::getCheckBoxWidget(QWidget* parent) { //체크박스 위젯을 테이블위젯에서 쓸 경우 가운데 정렬이 되도록 하는 함수
    QWidget* checkBoxWidget = new QWidget(parent);  //아이템에 Property를 추가하고 가운데 정렬이 되도록 함
    auto check_box = new QCheckBox(checkBoxWidget);                     //읽기/수정모드 체크하는 체크박스
    QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget);
    layoutCheckBox->addWidget(check_box);
    layoutCheckBox->setAlignment(Qt::AlignCenter);                      //위젯 안에서 가운데 정렬을 해야만 가운데 정렬이 됩니다
    layoutCheckBox->setContentsMargins(0,0,0,0);                        //마진을 없앱니다
    checkBoxWidget->setProperty("CB", QVariant::fromValue(check_box));  //CB에 체크박스 설정
    return checkBoxWidget;
}
QDateTimeEdit* View::getDateTimeEditWidget(const QDateTime &datetime, QWidget* parent) {    //product 조회 뷰에서 쓸 dateTimeEdit 위젯 생성하는 함수
    auto dt = new QDateTimeEdit(datetime, parent);
    dt->setReadOnly(true);
    return dt;
}

QTableWidgetItem* View::ceateTableItem(const QString id, QString title){ //아이템에 ID데이터를 추가하기
    auto item = new QTableWidgetItem(title);
    item->setData(Role::id, id);
    return item;
}
void CView::addClient(const QString ID, const QString name, const QString phone_number, const QString address){
    mgr.getCM().addClient(ID.toStdString(), name.toStdString(),phone_number.toStdString(),address.toStdString());
    notify<CView>();        //모든 뷰에 대해 고객 관리에 관련된 뷰라면 업데이트 한다
    notify<OView>();        //모든 뷰에 대해 주문 관리에 관련된 뷰라면 업데이트 한다 (구매 가능한 고객 목록 업데이트)
    notify<NView>();        //모든 뷰에 대해 채팅과 관련된 뷰라면 업데이트 한다 (채팅 참여자 목록에 추가가 가능하도록 업데이트)
}

bool CView::modifyClient(const QString id, const QList<QString> ls){
    bool re = mgr.getCM().modifyClient(id.toStdString(), CM::Client{id.toStdString(), ls[0].toStdString(),ls[1].toStdString(),ls[2].toStdString()});    //id는 바꾸지 않고 나머지 항목들에 대해 업데이트 한다(list로부터 값을 획득)
    notify<CView>();        //모든 뷰에 대해 고객 관리에 관련된 뷰라면 업데이트 한다
    notify<OView>();        //모든 뷰에 대해 주문 관리에 관련된 뷰라면 업데이트 한다
    return re;
}
bool CView::eraseClient(const QString id){
    bool result =  mgr.getCM().eraseClient(id.toStdString());       //고객 정보 삭제
    notify<CView>();        //모든 뷰에 대해 고객 관리에 관련된 뷰라면 업데이트 한다
    notify<OView>();        //모든 뷰에 대해 주문 관리에 관련된 뷰라면 업데이트 한다
    return result;
}

bool PView::addProduct(const QString name, const QString price, const QString qty){
    bool result = mgr.getPM().addProduct(name.toStdString(),price.toUInt(),qty.toUInt());
    notify<PView>();        //PView를 상속 받는 View 업데이트 (물품)
    notify<OView>();        //OView 상속 받는 View 업데이트 (주문)
    return result;
}
bool PView::modifyProduct(const QString id, const QList<QString> ls){
    tm time;
    std::istringstream ss( ls[3].toStdString() );       //물품 이름, 물품 가격, 물품 재고량, 물품 시각에서 3번째 시각 / Todo: ls[3]을 수정
    ss >> std::get_time(&time, "%D %T");                //시각 포맷
    bool re = mgr.getPM().modifyProduct(id.toStdString(), PM::Product{ls[0].toStdString(),ls[1].toUInt(),ls[2].toUInt(),time});
    notify<PView>();         //PView를 상속 받는 View 업데이트 (물품)
    notify<OView>();         //OView 상속 받는 View 업데이트 (주문)
    return re;
}
bool PView::eraseProduct(const QString id){
     bool result = mgr.getPM().eraseProduct(id.toStdString());
     notify<PView>();       //PView를 상속 받는 View 업데이트 (물품)
     notify<OView>();       //OView 상속 받는 View 업데이트 (주문)
     return result;
}
const PM::Product PView::findProduct(const QString id) const{
    return mgr.getPM().findProduct(id.toStdString());
}
const unsigned int PView::getSize() const{
    return mgr.getPM().getSize();
}

//OVIEW-----------------------------------------------------------------------------------
//bool OView::is_order_moified {false};
const OM::Order OView::findOrder(const OM::Order_ID order_id) const{
    return mgr.getOM().findOrder(order_id);
}
std::pair<const unsigned int, bool> OView::addOrder(const QString client_id, std::vector<OM::OrderManager::bill> products){
    auto result = mgr.getOM().addOrder(client_id.toStdString(),products);
    notify<OView>();    //PView를 상속 받는 View 업데이트 (물품)
    notify<PView>();    //OView 상속 받는 View 업데이트 (주문)
    return result;
}
const size_t OView::getSize() const{
    return mgr.getOM().getSize();
}
