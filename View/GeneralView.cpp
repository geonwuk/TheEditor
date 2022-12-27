#include "GeneralView.h"

#include <iomanip>
#include <sstream>

#include <QCheckBox>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QDateTimeEdit>

#include "mainwindow.h"

using std::vector;

View::View(MainManager& mgr, Tree& tree, const QIcon &icon=QPixmap(), const std::string label=std::string()) : icon{icon}, label{label}, mgr{mgr},tree{tree} {
    is_update=false;
    tab = new FocusTabItem{this,tree,icon,label};
    tree.getTabs().addChild(tab);
}
View::~View() {
    mgr.detachObserver(this);           //옵저버 패턴: 소멸 시 리스트에서 뺸다
    delete tab;
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

QTableWidgetItem* View::ceateTableItem(const std::string id, std::string title){ //아이템에 ID데이터를 추가하기
    auto item = new QTableWidgetItem(title.c_str());
    item->setData(Role::id, QString::fromStdString(id));
    return item;
}
void ClinetView::addClient(const std::string ID, const std::string name, const std::string phone_number, const std::string address){
    mgr.getClientModel().addClient(ID, name, phone_number, address);
    notify<ClinetView>();        //모든 뷰에 대해 고객 관리에 관련된 뷰라면 업데이트 한다
    notify<OrderView>();        //모든 뷰에 대해 주문 관리에 관련된 뷰라면 업데이트 한다 (구매 가능한 고객 목록 업데이트)
    notify<NetworkView>();        //모든 뷰에 대해 채팅과 관련된 뷰라면 업데이트 한다 (채팅 참여자 목록에 추가가 가능하도록 업데이트)
}

bool ClinetView::modifyClient(const std::string id, const vector<std::string> ls){
    bool re = mgr.getClientModel().modifyClient(id, CM::Client{id, ls[0],ls[1], ls[2]});    //id는 바꾸지 않고 나머지 항목들에 대해 업데이트 한다(list로부터 값을 획득)
    notify<ClinetView>();        //모든 뷰에 대해 고객 관리에 관련된 뷰라면 업데이트 한다
    notify<OrderView>();        //모든 뷰에 대해 주문 관리에 관련된 뷰라면 업데이트 한다
    return re;
}
bool ClinetView::eraseClient(const std::string id){
    bool result =  mgr.getClientModel().eraseClient(id);       //고객 정보 삭제
    notify<ClinetView>();        //모든 뷰에 대해 고객 관리에 관련된 뷰라면 업데이트 한다
    notify<OrderView>();        //모든 뷰에 대해 주문 관리에 관련된 뷰라면 업데이트 한다
    return result;
}

bool ProductView::addProduct(const std::string name, const std::string price, const std::string qty){
    bool result = mgr.getProductModel().addProduct(name, std::stoul(price), std::stoul(qty));
    notify<ProductView>();        //PView를 상속 받는 View 업데이트 (물품)
    notify<OrderView>();        //OView 상속 받는 View 업데이트 (주문)
    return result;
}
bool ProductView::modifyProduct(const std::string id, const vector<std::string> ls){
    tm time;
    std::istringstream ss( ls[3] );       //물품 이름, 물품 가격, 물품 재고량, 물품 시각에서 3번째 시각 / Todo: ls[3]을 수정
    ss >> std::get_time(&time, "%D %T");                //시각 포맷
    bool re = mgr.getProductModel().modifyProduct(id, PM::Product{ls[0],std::stoul(ls[1]),std::stoul(ls[2]),time});
    notify<ProductView>();         //PView를 상속 받는 View 업데이트 (물품)
    notify<OrderView>();         //OView 상속 받는 View 업데이트 (주문)
    return re;
}
bool ProductView::eraseProduct(const std::string id){
     bool result = mgr.getProductModel().eraseProduct(id);
     notify<ProductView>();       //PView를 상속 받는 View 업데이트 (물품)
     notify<OrderView>();       //OView 상속 받는 View 업데이트 (주문)
     return result;
}
const PM::Product ProductView::findProduct(const std::string id) const{
    return mgr.getProductModel().findProduct(id);
}
const unsigned int ProductView::getSize() const{
    return mgr.getProductModel().getSize();
}

//OVIEW-----------------------------------------------------------------------------------
//bool OView::is_order_moified {false};
const OM::Order OrderView::findOrder(const OM::Order_ID order_id) const{
    return mgr.getOrderModel().findOrder(order_id);
}
std::pair<const unsigned int, bool> OrderView::addOrder(const std::string client_id, std::vector<OM::OrderManager::bill> products){
    auto result = mgr.getOrderModel().addOrder(client_id,products);
    notify<OrderView>();    //PView를 상속 받는 View 업데이트 (물품)
    notify<ProductView>();    //OView 상속 받는 View 업데이트 (주문)
    return result;
}
const size_t OrderView::getSize() const{
    return mgr.getOrderModel().getSize();
}
