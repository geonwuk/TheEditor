#include "showview.h"
#include <QDate>
#include <cassert>


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
    QList<QString> labels{tr("Name"),tr("Price"),tr("Quantity"),tr("Date")};
    table->setColumnCount(labels.size());
    table->setHorizontalHeaderLabels(labels);
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
    orderInfoTable=ui.orderInfoTable;
    QList<QString> ls {tr("Order ID"),tr("Client"),tr("Price"),tr("Date")};
    orderTable->setColumnCount(ls.size());
    orderTable->setHorizontalHeaderLabels(ls);
    fillContents();
    label.append(tr("Show Order"));

    connect(orderTable,SIGNAL(itemSelectionChanged()),this,SLOT(orderItemSelectionChanged_()));



    QList<QString> pr_cell_names {tr("Product Name"),tr("Unit Price"),tr("Quantity"),tr("Total Price")};
    orderInfoTable->setColumnCount(pr_cell_names.size());
    orderInfoTable->setHorizontalHeaderLabels(pr_cell_names);

}

void ShowOrderView::fillContents() {

    orderTable->clearContents();
    orderTable->setRowCount(getSize());

    int i=0;
    for(const auto& order : getOrders()){
        qDebug()<<"filling order"<<i <<" ";
        int j=0;
        orderTable->setItem(i,j++,ceateTableItem(order.getID(), QString::number(order.getID())));
        auto client = mgr.getCM().findClient(order.getCID());
        orderTable->setItem(i,j++, new QTableWidgetItem(client.getName().c_str()));
        unsigned int price=0;
        for(auto ordered_product : order.getProductData()){
            auto product = mgr.getPM().findProduct(ordered_product.id);
            price += product.getPrice()*ordered_product.qty;
        }
        orderTable->setItem(i,j++, new QTableWidgetItem(QString::number(price)));
        auto tm = order.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon,tm.tm_mday};
        orderTable->setItem(i,j++,new QTableWidgetItem(date.toString()));
        i++;
    }
    orderTable->resizeColumnsToContents();
    orderTable->resizeRowsToContents();
}



void ShowOrderView::orderItemSelectionChanged_(){
    orderInfoTable->clearContents();
    auto ranges = orderTable->selectedRanges();
    if(ranges.empty()){
        orderInfoTable->setRowCount(0);
        return;
    }

    auto curr_row = orderTable->currentRow();
    auto item = orderTable->item(curr_row,0);
    OM::Order_ID order_id = item->data(Role::id).value<OM::Order_ID>();
    auto order = findOrder(order_id);
    assert(order!=OM::no_order);
    unsigned int total_price=0;
    auto product_data = order.getProductData();
    orderInfoTable->setRowCount(product_data.size());
    int i=0;
    for(auto pd : product_data){
        int j=0;
        auto product = mgr.getPM().findProduct(pd.id);
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(product.getName().c_str()));
        unsigned int price = product.getPrice();
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(QString::number(price)));
        unsigned int qty = pd.qty;
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(QString::number(qty)));
        unsigned int local_price = price*qty;
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(QString::number(local_price)));
        total_price+=local_price;
        i++;
    }






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
