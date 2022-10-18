#include "view.h"
#include "mainwindow.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTableWidgetItem>

//View::View() {}
View::View(Manager& mgr, Tree& tree, const QIcon icon, const QString label) : icon{icon}, label{label}, mgr{mgr},tree{tree} {
    tab = new FocusTabItem{this,tree,label};
    tree.getTabs().addChild(tab);
}
View::~View() {
    mgr.detachObserver(this);
    qDebug()<<"~VIEW: "<<label;


   // tree.getTabs().removeChild(tab);

}




QWidget* View::getCheckBoxWidget() {
    QWidget* checkBoxWidget = new QWidget();
    auto check_box = new QCheckBox;
    QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget);
    layoutCheckBox->addWidget(check_box);
    layoutCheckBox->setAlignment(Qt::AlignCenter);
    layoutCheckBox->setContentsMargins(0,0,0,0);
    checkBoxWidget->setProperty("CB", QVariant::fromValue(check_box));
    return checkBoxWidget;
}

QTableWidgetItem* View::ceateTableItem(const QString id, QString title){
    auto item = new QTableWidgetItem(title);
    item->setData(Role::id, id);
    qDebug()<<"ceate Item "<<id;
    return item;
}

bool CView::modifyClient(const QString id, const QList<QString> ls){
    bool re = mgr.getCM().modifyClient(id.toStdString(), CM::Client{ls[0].toStdString(),ls[1].toStdString(),ls[2].toStdString()});
    notify<CView>();
    notify<OView>();
    return re;
}
bool CView::eraseClient(const QString id)
{
    bool result =  mgr.getCM().eraseClient(id.toStdString());
    notify<CView>();
    notify<OView>();
    return result;
}

bool PView::addProduct(const QString name, const QString price, const QString qty){
    bool result = mgr.getPM().addProduct(name.toStdString(),price.toUInt(),qty.toUInt());
    auto c = *mgr.getPM().getProducts().begin();
    qDebug()<<c.getName().c_str()<<c.getPrice()<<c.getQty();
    notify<PView>();
    notify<OView>();
    return result;
}
bool PView::modifyProduct(const QString id, const QList<QString> ls){
    bool re = mgr.getPM().modifyProduct(id.toStdString(), PM::Product{ls[0].toStdString(),ls[1].toUInt(),ls[2].toUInt()});
    notify<PView>();
    notify<OView>();
    return re;
}
bool PView::eraseProduct(const QString id){
     bool result = mgr.getPM().eraseProduct(id.toStdString());
     notify<PView>();
     notify<OView>();
     return result;
}
const PM::Product& PView::findProduct(const QString id) const{
    return mgr.getPM().findProduct(id.toStdString());
}
PM::ProductManager::const_iterator PView::getProducts() const{
    return mgr.getPM().getProducts();
}

const unsigned int PView::getSize() const{
    return mgr.getPM().getSize();
}

//OVIEW-----------------------------------------------------------------------------------
bool OView::is_order_moified {false};
const OM::OrderManager::Order& OView::findOrder(const OM::Order_ID order_id) const{
    return mgr.getOM().findOrder(order_id);
}
std::pair<const unsigned int, bool> OView::addOrder(const QString client_id, std::vector<OM::OrderManager::bill> products){
    auto result = mgr.getOM().addOrder(client_id.toStdString(),products);
    notify<OView>();
    return result;
}
OM::OrderIterator OView::getOrders() const{
    return mgr.getOM().getOrders();
}
const size_t OView::getSize() const{
    return mgr.getOM().getSize();
}
