#include "addview.h"
#include "mainwindow.h"
#include <QDate>
#include <QCheckBox>
#include <QShortCut>
#include <QSpinBox>
AddClientView::AddClientView(Manager& mgr) : CView{mgr} {
    ui.setupUi(this);
    connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addClient()));
    label.append(tr("Add Client"));
}

void AddClientView::addClient(){
    qDebug()<<"add client";
    QString name = ui.NameEdit->text();
    QString phone_number = ui.PhoneNumberEdit->text();
    QString address = ui.AddressEdit->text();
    CView::addClient(name,phone_number,address);
}


AddClientView::~AddClientView(){

}

//Product
AddProductView::AddProductView(Manager& mgr) : PView{mgr}  {
    ui.setupUi(this);
    connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addProduct()));
    label.append(tr("Add Product"));
}

void AddProductView::addProduct(){
    qDebug()<<"add product";
    QString name = ui.NameEdit->text();
    QString price = ui.PriceEdit->text();
    QString qty = ui.QuantityEdit->text();
    PView::addProduct(name,price,qty);
}


AddProductView::~AddProductView(){

}


//Order
AddOrderView::AddOrderView(Manager& mgr) : OView{mgr} {
    ui.setupUi(this);
    CPTab=ui.CPTab;
    infoTab=ui.infoTable;
    orderTree=ui.orderTree;
    orderTree->setColumnCount(3);
    auto shortcut = new QShortcut(Qt::Key_Delete, orderTree, orderTree, [this](){
        qDebug()<<"delete tree item";
        delete orderTree->currentItem();
        orderTree->update();
    });

    fillClientTab();
    fillProductTab();
    CPTab->insertTab(0,&clientTab,tr("Client"));
    CPTab->insertTab(1,&productTab,tr("Product"));

    connect(ui.commitOrderButton,SIGNAL(pressed()),this,SLOT(commitOrder()));
    connect(ui.addOrderButton,SIGNAL(pressed()),this,SLOT(addOrder()));
    connect(&clientTab,SIGNAL(itemSelectionChanged()),this,SLOT(cleintItemSelectionChanged_()));
//    connect(&productTab, &QTableWidget::itemSelectionChanged, [this](){
//        itemSelectionChanged_(productTab);
//    });
    label.append(tr("Add Order"));
}


void AddOrderView::fillClientTab(){
    clientTab.clear();
    clientTab.setRowCount(mgr.getCM().getSize());
    clientTab.setColumnCount(4);
    clientTab.setHorizontalHeaderLabels({tr(""),tr("Name"),tr("Phone Number"),tr("Address")});

    int i=0;
    for(const auto& client : mgr.getCM().getCleints()){
        clientTab.setCellWidget(i,0, getCheckBoxWidget());
        clientTab.setItem(i,1,ceateTableItem(client.getId(), client.getName().c_str()));
        clientTab.setItem(i,2,ceateTableItem(client.getId(), client.getPhoneNumber().c_str()));
        clientTab.setItem(i,3,ceateTableItem(client.getId(), client.getAddress().c_str()));
        i++;
    }
    clientTab.resizeColumnsToContents();
    clientTab.resizeRowsToContents();
}

void AddOrderView::cleintItemSelectionChanged_(){
    std::vector<CM::CID> clients_ids;
    auto ranges = clientTab.selectedRanges();
    for(auto range : ranges){
        for(int top = range.topRow(); top<= range.bottomRow(); top++){
            auto item = clientTab.item(top,1);
            clients_ids.emplace_back(item->data(Role::id).value<CM::CID>());
        }
    }
    fillClientInfoTab(std::move(clients_ids));
}

std::vector<CM::CID> AddOrderView::itemSelectionChanged_(QTableWidget& table){
    std::vector<CM::CID> ids;
    auto ranges = table.selectedRanges();
    for(auto range : ranges){
        for(int top = range.topRow(); top<= range.bottomRow(); top++){
            auto item = table.item(top,1);
            ids.emplace_back(item->data(Role::id).value<CM::CID>());
        }
    }
    return ids;
}

void AddOrderView::fillClientInfoTab(std::vector<CM::CID> clients_ids){
    infoTab->clear();
    infoTab->setRowCount(clients_ids.size());
    infoTab->setColumnCount(3);
    int i=0;
    for(const auto& client_id : clients_ids){
        const auto client = mgr.getCM().findClient(client_id);
        infoTab->setItem(i,0, new QTableWidgetItem(client.getName().c_str()));
        infoTab->setItem(i,1, new QTableWidgetItem(client.getPhoneNumber().c_str()));
        infoTab->setItem(i,2, new QTableWidgetItem(client.getAddress().c_str()));
        i++;
    }
}

void AddOrderView::fillProductTab(){
    productTab.clear();
    productTab.setRowCount(mgr.getPM().getSize());
    productTab.setColumnCount(5);
    productTab.setHorizontalHeaderLabels({tr(""),tr("Name"),tr("Price"),tr("Qty"),tr("Date")});
    int i=0;
    for(const auto& product : mgr.getPM().getProducts()){
        productTab.setCellWidget(i,0, getCheckBoxWidget());
        productTab.setItem(i,1,ceateTableItem(product.getId(), product.getName().c_str()));
        productTab.setItem(i,2,ceateTableItem(product.getId(), QString::number(product.getPrice())));
        productTab.setItem(i,3,ceateTableItem(product.getId(), QString::number(product.getQty())));
        auto tm = product.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon,tm.tm_mday};
        productTab.setItem(i,4,ceateTableItem(product.getId(), date.toString()));
        i++;
    }
    productTab.resizeColumnsToContents();
    productTab.resizeRowsToContents();
}

//insertTab
std::vector<CM::CID> AddOrderView::getCheckedIDs(QTableWidget* table){
    std::vector<CM::CID> ids;
    for(int row=0; row<table->rowCount(); row++){
        auto wg = static_cast<QWidget*>(table->cellWidget(row,0));
        QVariant v = wg->property("CB");
        auto check_box = v.value<QCheckBox*>();
        auto state = check_box->checkState();
        if(state==Qt::Checked){
            auto item = table->item(row,1);
            ids.emplace_back(item->data(Role::id).value<CM::CID>());
        }
    }
    return ids;
}

void AddOrderView::commitOrder(){
    std::vector<CM::CID> client_ids = getCheckedIDs(&clientTab);
    std::vector<PM::PID> product_ids = getCheckedIDs(&productTab);

    std::vector<QTreeWidgetItem*> buyers;
    for(auto c : client_ids){
        QStringList name {mgr.getCM().findClient(c).getName().c_str()};
        auto buyer = new QTreeWidgetItem(orderTree,name);
        buyer->setData(0, Role::id, c);
        buyers.emplace_back(buyer);
        orderTree->addTopLevelItem(buyer);
        orderTree->expandItem(buyer);
    }

    for(auto p : product_ids){
        for(int row = 0; row < buyers.size(); row++){
            qDebug()<<"tree count"<<orderTree->topLevelItemCount();
            auto elem = buyers[row];
            auto product = new QTreeWidgetItem(elem);
            product->setData(0, Role::id, p);
            product->setText(1,mgr.getPM().findProduct(p).getName().c_str());
            QSpinBox* spin_box = new QSpinBox;
            spin_box->setValue(1);
            orderTree->setItemWidget(product,2,spin_box);
        }
    }
}

void AddOrderView::update(){
    if(!is_update){
        qDebug()<<"update false";
        fillClientTab();
        fillProductTab();
    }
    else{
        qDebug()<<"is_update : TRUE!";
    }
}

void AddOrderView::addOrder(){
    for(int i=0; i<orderTree->topLevelItemCount(); i++){
        auto client = orderTree->topLevelItem(i);
        CM::CID client_id = client->data(0, Role::id).value<CM::CID>();
        std::vector<OM::OrderManager::ProductData> products;
        for(int j=0; j<client->childCount(); j++){
            auto product = client->child(j);
            auto qty_box = static_cast<QSpinBox*>(orderTree->itemWidget(product,2));
            products.emplace_back(product->data(0, Role::id).value<PM::PID>(), qty_box->value());
        }
        if(products.empty())
            continue;
        OView::addOrder(client_id,std::move(products));

        OM::OrderManager::Order order = OView::findOrder(0);
        //qDebug()<<order.order_id<<order.client_id<<order.date.tm_mday<<order.products[0].qty;
    }
    orderTree->clear();
}



AddOrderView::~AddOrderView(){

}
