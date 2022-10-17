#include "addview.h"
#include "mainwindow.h"
#include <QDate>
#include <QCheckBox>
#include <QShortCut>
#include <QSpinBox>
AddClientView::AddClientView(Manager& mgr, Tree& tabs, const QIcon icon, const QString label) : CView{mgr,tabs,icon,label} {
    ui.setupUi(this);
    connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addClient()));
}

void AddClientView::addClient(){
    qDebug()<<"add client";
    QString ID = ui.IDLineEdit->text();
    QString name = ui.NameEdit->text();
    QString phone_number = ui.PhoneNumberEdit->text();
    QString address = ui.AddressEdit->text();
    CView::addClient(ID,name,phone_number,address);
}


AddClientView::~AddClientView(){

}

//Product
AddProductView::AddProductView(Manager& mgr, Tree& tabs, const QIcon icon, const QString label) : PView{mgr,tabs,icon,label}  {
    ui.setupUi(this);
    connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addProduct()));
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


//Order================================================================================
AddOrderView::AddOrderView(Manager& mgr, Tree& tabs, const QIcon icon, const QString label) : OView{mgr,tabs,icon,label} {
    ui.setupUi(this);
    CPTab=ui.CPTab;
    infoTab=ui.infoTable;
    orderTree=ui.orderTree;
    QList<QString> oderTable_labels {tr("Client"),tr("Product Name"),tr("Qty")};
    orderTree->setColumnCount(oderTable_labels.size());
    orderTree->setHeaderLabels(oderTable_labels);
    auto shortcut = new QShortcut(Qt::Key_Delete, orderTree, orderTree, [this](){
        qDebug()<<"delete tree item";
        delete orderTree->currentItem();
        orderTree->update();
    });

    fillClientTab();
    fillProductTab();
    cleintItemSelectionChanged_();
    CPTab->insertTab(0,&clientTab,tr("Client"));
    CPTab->insertTab(1,&productTab,tr("Product"));
    clientTab.setCornerButtonEnabled(true);
    clientTab.setStyleSheet("QTableCornerButton::section{border-width: 1px; border-color: #BABABA; border-style:solid;}");

    connect(ui.commitOrderButton,SIGNAL(pressed()),this,SLOT(commitOrder()));
    connect(ui.addOrderButton,SIGNAL(pressed()),this,SLOT(addOrder()));
    connect(&clientTab,SIGNAL(itemSelectionChanged()),this,SLOT(cleintItemSelectionChanged_()));
//    connect(&productTab, &QTableWidget::itemSelectionChanged, [this](){
//        itemSelectionChanged_(productTab);
//    });
    //label.append(tr("Add Order"));
}


void AddOrderView::fillClientTab(){
    clientTab.clear();
    clientTab.setRowCount(mgr.getCM().getSize());
    clientTab.setColumnCount(4);
    clientTab.setHorizontalHeaderLabels({tr(" "),tr("Name"),tr("Phone Number"),tr("Address")});

    int i=0;
    for(const auto& client : mgr.getCM().getCleints()){
        clientTab.setCellWidget(i,0, getCheckBoxWidget());
        clientTab.setItem(i,1,ceateTableItem(client.getId().c_str(), client.getName().c_str()));
        clientTab.setItem(i,2,new QTableWidgetItem(client.getPhoneNumber().c_str()));
        clientTab.setItem(i,3,new QTableWidgetItem(client.getAddress().c_str()));
        i++;
    }
    clientTab.resizeColumnsToContents();
    clientTab.resizeRowsToContents();
}

void AddOrderView::cleintItemSelectionChanged_(){
    QList<QString> clients_ids;
    auto ranges = clientTab.selectedRanges();
    if(ranges.empty()){
        infoTab->clear();
        infoTab->setColumnCount(1);
        infoTab->setRowCount(0);
        infoTab->setHorizontalHeaderLabels({tr("No client or product item selected")});
        infoTab->resizeColumnsToContents();
        return;
    }
    for(auto range : ranges){
        for(int top = range.topRow(); top<= range.bottomRow(); top++){
            auto item = clientTab.item(top,1);
            clients_ids.emplace_back(item->data(Role::id).value<QString>());
        }
    }
    fillClientInfoTab(std::move(clients_ids));
}


void AddOrderView::fillClientInfoTab(QList<QString> clients_ids){
    infoTab->clear();
    infoTab->setRowCount(clients_ids.size());
    infoTab->setColumnCount(3);
    infoTab->setHorizontalHeaderLabels({tr("Name"),tr("Phone Number"),tr("Address")});
    int i=0;
    for(const auto& client_id : clients_ids){
        const auto client = mgr.getCM().findClient(client_id.toStdString());
        infoTab->setItem(i,0, new QTableWidgetItem(client.getName().c_str()));
        infoTab->setItem(i,1, new QTableWidgetItem(client.getPhoneNumber().c_str()));
        infoTab->setItem(i,2, new QTableWidgetItem(client.getAddress().c_str()));
        i++;
    }
    infoTab->resizeColumnsToContents();
    infoTab->resizeRowsToContents();
}

void AddOrderView::fillProductTab(){
    productTab.clear();
    productTab.setRowCount(mgr.getPM().getSize());
    productTab.setColumnCount(5);
    productTab.setHorizontalHeaderLabels({tr(""),tr("Name"),tr("Price"),tr("Qty"),tr("Date")});
    int i=0;
    for(const auto& product : mgr.getPM().getProducts()){
        productTab.setCellWidget(i,0, getCheckBoxWidget());
        productTab.setItem(i,1,ceateTableItem(product.getId().c_str(), product.getName().c_str()));
        productTab.setItem(i,2,new QTableWidgetItem(QString::number(product.getPrice())));
        productTab.setItem(i,3,new QTableWidgetItem(QString::number(product.getQty())));
        auto tm = product.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon,tm.tm_mday};
        productTab.setItem(i,4,new QTableWidgetItem(date.toString()));
        i++;
    }
    productTab.resizeColumnsToContents();
    productTab.resizeRowsToContents();
}

//insertTab
std::vector<QString> AddOrderView::getCheckedIDs(QTableWidget* table){
    std::vector<QString> ids;
    for(int row=0; row<table->rowCount(); row++){
        auto wg = static_cast<QWidget*>(table->cellWidget(row,0));
        QVariant v = wg->property("CB");
        auto check_box = v.value<QCheckBox*>();
        auto state = check_box->checkState();
        if(state==Qt::Checked){
            auto item = table->item(row,1);
            ids.emplace_back(item->data(Role::id).value<QString>());
        }
    }
    return ids;
}

void AddOrderView::commitOrder(){
    std::vector<QString> client_ids = getCheckedIDs(&clientTab);
    std::vector<QString> product_ids = getCheckedIDs(&productTab);

    std::vector<QTreeWidgetItem*> buyers;
    for(const auto& c : client_ids){
        QStringList name {mgr.getCM().findClient(c.toStdString()).getName().c_str()};
        auto buyer = new QTreeWidgetItem(orderTree,name);
        buyer->setData(0, Role::id, c);
        buyers.emplace_back(buyer);
        orderTree->addTopLevelItem(buyer);
        orderTree->expandItem(buyer);
    }

    for(const auto& p : product_ids){
        for(int row = 0; row < buyers.size(); row++){
            qDebug()<<"tree count"<<orderTree->topLevelItemCount();
            auto elem = buyers[row];
            auto product = new QTreeWidgetItem(elem);
            product->setData(0, Role::id, p);
            product->setText(1,mgr.getPM().findProduct(p.toStdString()).getName().c_str());
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
        QString client_id = client->data(0, Role::id).value<QString>();
        std::vector<OM::OrderManager::bill> products;
        for(int j=0; j<client->childCount(); j++){
            auto product = client->child(j);
            auto qty_box = static_cast<QSpinBox*>(orderTree->itemWidget(product,2));
            QString pid = product->data(0, Role::id).value<QString>();
            products.emplace_back(pid.toStdString(), qty_box->value());
        }
        if(products.empty())
            continue;
        OView::addOrder(client_id,std::move(products));

        //OM::OrderManager::Order order = OView::findOrder(0);
        //qDebug()<<"ORDER"<<order.getID()<<order.getClient().getId()<<order.getProductData()[0].qty<<mgr.getOM().getSize();
    }
    orderTree->clear();
}



AddOrderView::~AddOrderView(){

}




AddParticipantView::AddParticipantView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label)
    : NView{mgr, tabs, icon,label} {
    ui.setupUi(this);
    ui.addButton->setIcon(ui.addButton->style()->standardIcon(QStyle::SP_ArrowRight));
    ui.addButton->setIconSize(QSize{64,64});
    ui.dropButton->setIcon(ui.dropButton->style()->standardIcon(QStyle::SP_ArrowLeft));
    ui.dropButton->setIconSize(QSize{64,64});

    ui.verticalLayout_2->SetFixedSize();

    ui.splitter->setCollapsible(1,false);
    ui.splitter->setStretchFactor(0,1);
    ui.splitter->setStretchFactor(1,0);
    ui.splitter->setStretchFactor(2,1);

    ui.splitter->setSizes({1,0,1});


    ui.clientList->setColumnCount(3);
    ui.clientList->setHeaderLabels({tr("ID"),tr("Name"),tr("Status")});
    fillContents();

    connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addParticipant()));
    connect(ui.dropButton,SIGNAL(pressed()),this,SLOT(dropParticipant()));
}

void AddParticipantView::addParticipant(){

}
void AddParticipantView::dropParticipant(){

}
void AddParticipantView::fillContents(){
    ui.clientList->clear();
    for(const auto& nclient : mgr.getSM().net_clients){
        QList<QString> ls;
        ls<<nclient.self->getId().c_str()<<nclient.self->getName().c_str()<<nclient.is_online;
        auto item = new QTreeWidgetItem(ls);
        ui.clientList->addTopLevelItem(item);
    }
}

AddParticipantView::~AddParticipantView(){}


void AddParticipantView::update(){
    qDebug()<<"NVIew update";
    fillContents();
}
