#include "showview.h"
#include <QDate>
#include <cassert>
#include <QShortcut>


ShowClientView::ShowClientView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label) : CView{mgr, tabs,icon,label}
{
    ui.setupUi(this);
    table=ui.tableWidget;
    editBox=ui.checkBox;
    searchLineEdit=ui.lineEdit;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QList<QString> header_labels {tr("ID"),tr("Name"),tr("Phone Number"),tr("Address")};
    table->setColumnCount(header_labels.size());
    table->setHorizontalHeaderLabels(header_labels);
    fillContents();
    connect(ui.checkBox,&QCheckBox::stateChanged, [=](int status){
       is_edit_mode=status;
       is_edit_mode ? table->setEditTriggers(QAbstractItemView::AllEditTriggers) : table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    });
    connect(table,SIGNAL(cellChanged(int,int)),SLOT(cellChanged(int,int)));
    connect(searchLineEdit,SIGNAL(returnPressed()),SLOT(returnPressed()));
    auto shortcut = new QShortcut(Qt::Key_Delete, table, table, [this](){
            if(!is_edit_mode) return;
        eraseClient(table->currentRow());
    });
}
void ShowClientView::fillContents(){
    table->blockSignals(true);
    table->clearContents();
    table->setRowCount(getSize());
    int i=0;
    for(const auto& client : getCleints()){
        int j=0;
        table->setItem(i,j++,ceateTableItem(client.getId().c_str(), client.getId().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(client.getName().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(client.getPhoneNumber().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(client.getAddress().c_str()));
        i++;
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->blockSignals(false);
}

void ShowClientView::update(){
    fillContents();
    qDebug()<<"Show CLient view update";
}
void ShowClientView::cellChanged(int row, int col){
    if(is_edit_mode){
        auto id_item = table->item(row,0);
        QString id = id_item->data(Role::id).value<QString>();

        QList<QString> ls;
        int d = table->columnCount();
        for(int i=1; i<d; i++){
            qDebug()<<"i: "<<i<<row<<col;
            ls<<table->item(row, i)->text();
        }
        //CM::Client client {0,ls[0],ls[1],ls[3]};
        table->blockSignals(true);
        modifyClient(id,ls);
        table->blockSignals(false);
    }
    qDebug()<<"end222";
}
void ShowClientView::returnPressed(){
    qDebug()<<"return pressed;";
    fillContents();
    QString str = searchLineEdit->text();
    QList<int> rows_to_delete;
    for(int row=0; row<table->rowCount(); row++){
        QStringList ls;
        for(int col=0; col<table->columnCount(); col++){
            ls<<table->item(row,col)->text();
        }
        for(const auto &e : ls){
            if(e.contains(str)) goto end;
        }
        rows_to_delete<<row;
        end:;
    }
    int compensation=0;
    for(auto row:rows_to_delete){
        table->removeRow(row-compensation);
        compensation++;
    }
}

bool ShowClientView::eraseClient(int row){
    auto item = table->item(row,id_col);
    QString id = item->data(Role::id).value<QString>();
    bool result = CView::eraseClient(id);
     qDebug()<<"delete table item"<<result;
    if(!result) return false;
    table->removeRow(row);
    table->update();
    return result;
}

ShowClientView::~ShowClientView(){

}





//Product_------------------------------------------------------------
ShowProductView::ShowProductView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label) : PView{mgr, tabs,icon,label} {
    ui.setupUi(this);
    table=ui.tableWidget;
    QList<QString> labels{tr("ID"),tr("Name"),tr("Price"),tr("Quantity"),tr("Date")};
    table->setColumnCount(labels.size());
    table->setHorizontalHeaderLabels(labels);
    fillContents();
    //label.append(tr("Show Product"));
}
void ShowProductView::fillContents(){
    table->clearContents();
    table->setRowCount(getSize());
    int i=0;
    for(const auto& product : getProducts()){
        int j =0;
        table->setItem(i,j++,ceateTableItem(product.getId().c_str(), product.getId().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(product.getName().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(QString::number(product.getPrice())));
        table->setItem(i,j++,new QTableWidgetItem(QString::number(product.getQty())));
        auto tm = product.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon,tm.tm_mday};
        table->setItem(i,j++,new QTableWidgetItem(date.toString()));
        i++;
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

void ShowProductView::update(){
    if(!is_update){
        qDebug()<<"product update false";
        fillContents();
    }
    else{
        qDebug()<<"prodcut is_update : TRUE!";
    }
}

ShowProductView::~ShowProductView(){}

//ORDER-------------------------------------------------------------------------------------------------------
ShowOrderView::ShowOrderView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label) : OView{mgr, tabs,icon,label} {
    ui.setupUi(this);
    orderTable=ui.orderTable;
    orderInfoTable=ui.orderInfoTable;
    QList<QString> ls {tr("Order ID"),tr("Client"),tr("Price"),tr("Date")};
    orderTable->setColumnCount(ls.size());
    orderTable->setHorizontalHeaderLabels(ls);
    fillContents();
    //label.append(tr("Show Order"));

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
        orderTable->setItem(i,j++,ceateTableItem(QString::number(order.getID()), QString::number(order.getID())));
        auto client = order.getClient();
        orderTable->setItem(i,j++, new QTableWidgetItem(client.getName().c_str()));
        unsigned int price=0;
        for(auto ordered_product : order.getProducts()){
            auto product = ordered_product.product;
            price += product->getPrice()*ordered_product.qty;
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
    auto product_data = order.getProducts();
    orderInfoTable->setRowCount(product_data.size());
    int i=0;
    for(auto pd : product_data){
        int j=0;
        auto product = pd.product;
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(product->getName().c_str()));
        unsigned int price = product->getPrice();
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
