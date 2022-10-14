#include "view.h"
#include "mainwindow.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTableWidgetItem>
//View::View() {}

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

QTableWidgetItem* View::ceateTableItem(const CM::CID id, QString title){
    auto item = new QTableWidgetItem(title);
    item->setData(Role::id, id);
    return item;
}

bool PView::addProduct(const QString name, const QString price, const QString qty){
    bool result = mgr.getPM().addProduct(name.toStdString(),price.toUInt(),qty.toUInt());
    auto c = *mgr.getPM().getProducts().begin();
    qDebug()<<c.getName().c_str()<<c.getPrice()<<c.getQty();
    notify<PView>();
    notify<OView>();
    return result;
}

bool PView::eraseProduct(const PM::PID id){
     bool result = mgr.getPM().eraseProduct(id);
     notify<PView>();
     notify<OView>();
     return result;
}
const PM::Product& PView::findProduct(const PM::PID id) const{
    return mgr.getPM().findProduct(id);
}
PM::ProductManager::const_iterator PView::getProducts() const{
    return mgr.getPM().getProducts();
}
const PM::Product& PView::getProduct(const PM::PID pid) const{
    return mgr.getPM().getProduct(pid);
}

const unsigned int PView::getSize() const{
    return mgr.getPM().getSize();
}

//OVIEW-----------------------------------------------------------------------------------
bool OView::is_order_moified {false};
const OM::OrderManager::Order& OView::findOrder(const OM::Order_ID order_id) const{
    return mgr.getOM().findOrder(order_id);
}
std::pair<const unsigned int, bool> OView::addOrder(const OM::Client_ID client_id, std::vector<OM::OrderManager::ProductData> products){
    auto result = mgr.getOM().addOrder(client_id,products);
    notify<OView>();
    return result;
}
OM::OrderIterator OView::getOrders() const{
    return mgr.getOM().getOrders();
}
const unsigned int OView::getSize() const{
    return mgr.getOM().getSize();
}
