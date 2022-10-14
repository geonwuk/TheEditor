#include "showview.h"
#include <QDate>

enum Role {id = Qt::UserRole};

ShowClientView::ShowClientView(Manager& mgr) : CView{mgr} {
    ui.setupUi(this);
    table=ui.tableWidget;
    table->setColumnCount(col);
    table->setHorizontalHeaderLabels({tr("Name"),tr("Phone Number"),tr("Address")});
    fillContents();
    label.append(tr("Show Client"));
}
void ShowClientView::fillContents(){
    table->clearContents();
    table->setRowCount(getSize());
    int i=0;
    for(const auto& client : getCleints()){
        table->setItem(i,0,ceateTableItem(client.getId(), client.getName().c_str()));
        table->setItem(i,1,ceateTableItem(client.getId(), client.getPhoneNumber().c_str()));
        table->setItem(i,2,ceateTableItem(client.getId(), client.getAddress().c_str()));
        i++;
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

void ShowClientView::update(){
    if(!is_update){
        qDebug()<<"update false";
        fillContents();
    }
    else{
        qDebug()<<"is_update : TRUE!";
    }
}

ShowClientView::~ShowClientView(){

}


ShowProductView::ShowProductView(Manager& mgr) : PView{mgr} {
    ui.setupUi(this);
    table=ui.tableWidget;
    table->setColumnCount(col);
    table->setHorizontalHeaderLabels({tr("Name"),tr("Price"),tr("Quantity")});
    fillContents();
    label.append(tr("Show Product"));
}
void ShowProductView::fillContents(){
    table->clearContents();
    table->setRowCount(getSize());
    int i=0;
    for(const auto& product : getProducts()){
        table->setItem(i,0,ceateTableItem(product.getId(), product.getName().c_str()));
        table->setItem(i,1,ceateTableItem(product.getId(), QString::number(product.getPrice())));
        table->setItem(i,2,ceateTableItem(product.getId(), QString::number(product.getQty())));
        auto tm = product.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon,tm.tm_mday};
        table->setItem(i,3,ceateTableItem(product.getId(), date.toString()));
        i++;
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

void ShowProductView::update(){
    if(!is_update){
        qDebug()<<"product update false";
        fillContents();
        is_update=true;
    }
    else{
        qDebug()<<"prodcut is_update : TRUE!";
    }
}

ShowProductView::~ShowProductView(){}

//ORDER-------------------------------------------------------------------------------------------------------
ShowOrderView::ShowOrderView(Manager& mgr) : OView{mgr} {
    ui.setupUi(this);
    orderTable=ui.orderTable;
    oderInfoTable=ui.oderInfoTable;
    orderTable->setColumnCount(5);
    orderTable->setHorizontalHeaderLabels({tr(""),tr("Order ID"),tr("Client"),tr("Price"),tr("Date")});
    fillContents();
    label.append(tr("Show Order"));

    connect(orderTable,SIGNAL(itemSelectionChanged()),this,SLOT(cleintItemSelectionChanged_()));
}

void ShowOrderView::fillContents() {

    orderTable->clearContents();
    orderTable->setRowCount(getSize());

    int i=0;
    for(const auto& order : getOrders()){
        orderTable->setCellWidget(i,0, getCheckBoxWidget());
        orderTable->setItem(i,1,ceateTableItem(order.getID(), QString::number(order.getID())));
        auto client = mgr.getCM().findClient(order.getCID());
        orderTable->setItem(i,2, new QTableWidgetItem(client.getName().c_str()));
        unsigned int price=0;
        for(auto ordered_product : order.getProducts()){
            auto product = mgr.getPM().findProduct(ordered_product.id);
            price += product.getPrice()*ordered_product.qty;
        }
        orderTable->setItem(i,3, new QTableWidgetItem(QString::number(price)));
        auto tm = order.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon,tm.tm_mday};
        orderTable->setItem(i,4,new QTableWidgetItem(date.toString()));
        i++;
    }
    orderTable->resizeColumnsToContents();
    orderTable->resizeRowsToContents();
}

void ShowOrderView::cleintItemSelectionChanged_(){

}


ShowOrderView::~ShowOrderView(){}
void ShowOrderView::update(){
    if(!is_update){
        qDebug()<<"show order updated";
        fillContents();
    }
    else{
        qDebug()<<"show order is_update : TRUE!";
    }
}
