#include "addview.h"

#include <QDate>
#include <QCheckBox>
#include <QShortCut>
#include <QSpinBox>
#include <QMessageBox>
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
        delete orderTree->currentItem();
        orderTree->update();
    });
    clientTab.setObjectName("clientTab");
    productTab.setObjectName("productTab");

    fillClientTab();
    fillProductTab();
    infoTab->clear();
    infoTab->setColumnCount(1);
    infoTab->setRowCount(0);
    infoTab->setHorizontalHeaderLabels({tr("No client or product item selected")});
    infoTab->resizeColumnsToContents();
    CPTab->insertTab(0,&clientTab,tr("Client"));
    CPTab->insertTab(1,&productTab,tr("Product"));
    clientTab.setCornerButtonEnabled(true);


    connect(ui.commitOrderButton,SIGNAL(pressed()),this,SLOT(commitOrder()));
    connect(ui.addOrderButton,SIGNAL(pressed()),this,SLOT(addOrder()));
    connect(&clientTab,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged_()));
    connect(&productTab,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged_()));
//    connect(&productTab, &QTableWidget::itemSelectionChanged, [this](){
//        itemSelectionChanged_(productTab);
//    });
    //label.append(tr("Add Order"));
    ui.splitter_2->setSizes({ui.CPTab->sizeHint().width(),ui.orderTree->sizeHint().width()});

}


void AddOrderView::fillClientTab(){
    clientTab.clear();
    clientTab.setRowCount(mgr.getCM().getSize());
    clientTab.setColumnCount(3);
    clientTab.setHorizontalHeaderLabels({tr("Select"),tr("ID"),tr("Name")});

    int i=0;
    for(const auto& client : mgr.getCM().getCleints()){
        clientTab.setCellWidget(i,0, getCheckBoxWidget(this));
        clientTab.setItem(i,1,ceateTableItem(client.getId().c_str(), client.getId().c_str()));
        clientTab.setItem(i,2,new QTableWidgetItem(client.getName().c_str()));
        i++;
    }
    clientTab.resizeColumnsToContents();
    clientTab.resizeRowsToContents();
}


void AddOrderView::itemSelectionChanged_(){
    auto tab = qobject_cast<QTableWidget*>(sender());
    if(tab==nullptr)
        return;
    QList<QString> ids;
    auto ranges = tab->selectedRanges();
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
            auto item = tab->item(top,1);
            ids.emplace_back(item->data(Role::id).value<QString>());
        }
    }
    if(tab->objectName()=="clientTab"){
        fillClientInfoTab(std::move(ids));
    }
    else{
        fillProductInfoTab(std::move(ids));
    }

}

void AddOrderView::fillClientInfoTab(QList<QString> clients_ids){
    infoTab->clear();
    infoTab->setRowCount(clients_ids.size());
    infoTab->setColumnCount(4);
    infoTab->setHorizontalHeaderLabels({tr("ID"),tr("Name"),tr("Phone Number"),tr("Address")});
    int i=0;
    for(const auto& client_id : clients_ids){
        const auto client = mgr.getCM().findClient(client_id.toStdString());
        infoTab->setItem(i,0, new QTableWidgetItem(client.getId().c_str()));
        infoTab->setItem(i,1, new QTableWidgetItem(client.getName().c_str()));
        infoTab->setItem(i,2, new QTableWidgetItem(client.getPhoneNumber().c_str()));
        infoTab->setItem(i,3, new QTableWidgetItem(client.getAddress().c_str()));
        i++;
    }
    infoTab->resizeColumnsToContents();
    infoTab->resizeRowsToContents();
}

void AddOrderView::fillProductInfoTab(QList<QString> product_ids){
    infoTab->clear();
    infoTab->setRowCount(product_ids.size());
    infoTab->setColumnCount(5);
    infoTab->setHorizontalHeaderLabels({tr("ID"),tr("Name"),tr("Price"),tr("Qty"),tr("Date")});
    int i=0;
    for(const auto& product_id : product_ids){
        const auto product = mgr.getPM().findProduct(product_id.toStdString());
        infoTab->setItem(i,0, new QTableWidgetItem(product.getId().c_str()));
        infoTab->setItem(i,1, new QTableWidgetItem(product.getName().c_str()));
        infoTab->setItem(i,2,new QTableWidgetItem(QString::number(product.getPrice())));
        infoTab->setItem(i,3,new QTableWidgetItem(QString::number(product.getQty())));
        auto tm = product.getDate();
        QDate date {tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday};
        QTime time {tm.tm_hour,tm.tm_min,tm.tm_sec};
        QDateTime dateTime {date,time};
        infoTab->setItem(i,4,new QTableWidgetItem(dateTime.toString("MM/dd/yy hh:mm:ss")));
        i++;
    }
    infoTab->resizeColumnsToContents();
    infoTab->resizeRowsToContents();
}

void AddOrderView::fillProductTab(){
    productTab.clear();
    productTab.setRowCount(mgr.getPM().getSize());
    productTab.setColumnCount(3);
    productTab.setHorizontalHeaderLabels({tr("Select"),tr("ID"),tr("Name")});
    int i=0;
    for(const auto& product : mgr.getPM().getProducts()){
        productTab.setCellWidget(i,0, getCheckBoxWidget(this));
        productTab.setItem(i,1,ceateTableItem(product.getId().c_str(), product.getId().c_str()));
        productTab.setItem(i,2,new QTableWidgetItem(product.getName().c_str()));
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
    if(orderTree->topLevelItemCount()==0){
        QMessageBox::warning(this, tr("Warning"), tr("No order list was created\n Commit order first."));
    }
    for(int i=0; i<orderTree->topLevelItemCount(); i++){
        auto client = orderTree->topLevelItem(i);
        QString client_id = client->data(0, Role::id).value<QString>();
        std::vector<OM::OrderManager::bill> products;
        for(int j=0; j<client->childCount(); j++){
            auto product = client->child(j);
            auto qty_box = static_cast<QSpinBox*>(orderTree->itemWidget(product,2));
            QString pid = product->data(0, Role::id).value<QString>();
            auto target_product = mgr.getPM().findProduct(pid.toStdString());
            if(target_product.getQty()<qty_box->value()){
                QMessageBox::warning(this, tr("Warning"), tr("%1's total stock is less than %2").arg(QString(target_product.getName().c_str())).arg(qty_box->value()));
                return;
            }

            products.emplace_back(pid.toStdString(), qty_box->value());
        }
        bool result;
        std::tie(std::ignore,result)=OView::addOrder(client_id,std::move(products));
        assert(result);
    }
    orderTree->clear();
}



AddOrderView::~AddOrderView(){

}
#include "Network/servermanager.h"
static QTreeWidgetItem* createTreeItem(const unsigned int col, const QString id, QList<QString> titles){
    auto item = new QTreeWidgetItem(titles);
    item->setData(col, Qt::UserRole, id);
    return item;
}
AddParticipantView::AddParticipantView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label)
    : NView{mgr, tabs, icon,label} {
    initUI();

    fillContents();

    connect(ui.addButton,SIGNAL(clicked()),this,SLOT(addParticipant()));
    connect(ui.dropButton,SIGNAL(clicked()),this,SLOT(dropParticipant()));
}
void AddParticipantView::initUI(){
    ui.setupUi(this);
    ui.addButton->setIcon(ui.addButton->style()->standardIcon(QStyle::SP_ArrowRight));
    ui.addButton->setIconSize(QSize{32,32});
    ui.dropButton->setIcon(ui.dropButton->style()->standardIcon(QStyle::SP_ArrowLeft));
    ui.dropButton->setIconSize(QSize{32,32});

    QSplitter* splitter = new QSplitter(this);

    auto left = new QWidget(this);
    left->setLayout(ui.clientLayout);
    left->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    auto middle = new QWidget(this);
    middle->setLayout(ui.buttonLayout);
    middle->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
    auto right = new QWidget(this);
    right->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    right->setLayout(ui.participantsLayout);

    splitter->addWidget(left);
    splitter->addWidget(middle);
    splitter->addWidget(right);

    auto center_layout = new QHBoxLayout;
    center_layout->addWidget(splitter);
    setLayout(center_layout);


    splitter->setCollapsible(1,false);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,0);
    splitter->setStretchFactor(2,1);
    splitter->setHandleWidth(1);
    //splitter->setSizes({1,0,1});

    QList<QString> headers {tr("ID"),tr("Name")};
    ui.clientList->setColumnCount(headers.size());
    ui.clientList->setHorizontalHeaderLabels(headers);
}

void AddParticipantView::addParticipant(){
    auto ranges = ui.clientList->selectedRanges();
    if(ranges.empty()){
        QMessageBox::warning(this,tr("Warning"),tr("Please select clients"));
        return;
    }
    auto chat_rooms =ui.chatRoomTree->selectedItems();
    if(chat_rooms.empty()){
        QMessageBox::warning(this,tr("Warning"),tr("Please select chat rooms"));
        return;
    }
    for(auto chat_room : chat_rooms){
        if(chat_room->parent()!=nullptr){
            QMessageBox::warning(this,tr("Warning"),tr("Please select chat rooms only"));
            return;
        }
    }

    for(auto range : ranges){
        for(int top = range.topRow(); top<= range.bottomRow(); top++){
            auto id_item = ui.clientList->item(top,0);
            QString id = id_item->data(Role::id).value<QString>();
            auto client = mgr.getCM().copyClient(id.toStdString());
            NetClient&  nc = mgr.getSM().addClient(client);

            auto chat_rooms = ui.chatRoomTree->selectedItems();
            for(auto& chat_room : chat_rooms){
                auto chat_room_no = chat_room->data(0,Role::id).value<unsigned int>();
                mgr.getSM().attendClient(chat_room_no, nc);
            }
        }
    }

    fillContents();
    notify<NView>();
}
void AddParticipantView::dropParticipant(){
    auto ls = ui.chatRoomTree->selectedItems();
    if(ls.empty()){
        QMessageBox::warning(this,tr("Warning"),tr("Please select Participants"));
        return;
    }
    for(const auto& participants : ls){
        if(participants->parent()==nullptr){
             QMessageBox::warning(this, tr("Warning"), tr("Please Select Participants Only."));
             return;
        }
    }
    for(const auto& participant : ls){
        auto chat_room = participant->parent();
        unsigned int chat_room_no = chat_room->data(0,Role::id).value<QString>().toInt();
        QString id = participant->data(1, Role::id).value<QString>();
        mgr.getSM().dropClient(chat_room_no,id);
    }
    fillContents();
    notify<NView>();
}


void AddParticipantView::fillContents(){
    ui.clientList->clearContents();
    ui.clientList->setRowCount(mgr.getCM().getSize());
    int i=0;
    auto participants = mgr.getSM().getNetClients();
    for(auto client = mgr.getCM().getCleints().begin(); client!=mgr.getCM().getCleints().end(); ++client){
        int j=0;
        ui.clientList->setItem(i,j++,ceateTableItem(client->getId().c_str(), client->getId().c_str()) );
        ui.clientList->setItem(i,j++,new QTableWidgetItem(client->getName().c_str()));
        i++;
    }
    ui.clientList->setRowCount(i);
    ui.clientList->resizeColumnsToContents();
    ui.clientList->resizeRowsToContents();

    ui.chatRoomTree->clear();
    for(const auto& chat_room_info : mgr.getSM().getChatRooms()){
        const auto chat_room_id = chat_room_info.getId();
        const auto& chat_room = chat_room_info;
        auto item = createTreeItem(0,QString::number(chat_room_id),{chat_room.getName()});
        ui.chatRoomTree->addTopLevelItem(item);
        for(const NetClient* participant : chat_room.getParticipants()){
            qDebug()<<"attendants"<<participant->getClient().getId().c_str();
            item->addChild(createTreeItem(1,participant->getClient().getId().c_str(),{"",participant->getClient().getId().c_str(),participant->getClient().getName().c_str()}));
        }
        ui.chatRoomTree->expandItem(item);
    }
    for(int i=0; i<ui.chatRoomTree->columnCount(); i++){
        ui.chatRoomTree->resizeColumnToContents(i);
    }
}
#include <QInputDialog>
#include "View/showview.h"
void AddParticipantView::on_addChatRoomButton_clicked(){
    bool ok;
    QString text = QInputDialog::getText(nullptr, tr("Add Chat Room"),
                                         tr("Chat Room name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()){
        auto chat_room = mgr.getSM().addChatRoom(text.toStdString());
        ViewFactory* showChat {new ViewMaker<ShowChatView>{mgr,QIcon(":/Icons/chat.png"),QString::number(chat_room.getId())+','+chat_room.getName()}};
        QTreeWidgetItem* chat_room_view = new ToTabItem(showChat, tree, showChat->getIcon(), chat_room.getName());
        tree.topLevelItem(0)->child(1)->addChild(chat_room_view);
        tree.expandAll();
        fillContents();
        notify<NView>();
    }
}

AddParticipantView::~AddParticipantView(){}



void AddParticipantView::update(){
    fillContents();
}
