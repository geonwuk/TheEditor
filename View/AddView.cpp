#include "AddView.h"
#include "mainwindow.h"
#include <QDate>
#include <QCheckBox>
#include <QShortcut>
#include <QSpinBox>
#include <QMessageBox>
AddClientView::AddClientView(Manager& mgr, Tree& tabs, const QIcon icon, const QString label) : CView{mgr,tabs,icon,label} {
    ui.setupUi(this);
    assert(connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addClient()))); //고객 추가 버튼과 고객 추가 메소드를 시그널&슬롯으로 연결합니다
}

void AddClientView::addClient(){                                //고객 추가 UI에 있는 QLineEdit으로부터 스트링 데이터를 얻어서 Manager클래스의 Map에 추가합니다.
    QString ID = ui.IDLineEdit->text();                         //ID 스트링 데이터를 QLineEdit으로부터 얻습니다
    QString name = ui.NameEdit->text();                         //name 스트링 데이터를 QLineEdit으로부터 얻습니다
    QString phone_number = ui.PhoneNumberEdit->text();          //전화번호 스트링 데이터를 QLineEdit으로부터 얻습니다
    QString address = ui.AddressEdit->text();                   //주소 스트링 데이터를 QLineEdit으로부터 얻습니다
    CView::addClient(ID,name,phone_number,address);
}


AddClientView::~AddClientView(){

}

//Product
AddProductView::AddProductView(Manager& mgr, Tree& tabs, const QIcon icon, const QString label) : PView{mgr,tabs,icon,label}  {
    ui.setupUi(this);
    assert(connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addProduct())));    //고객 추가 버튼과 고객 추가 메소드를 시그널&슬롯으로 연결합니다
}

void AddProductView::addProduct(){              //상품 추가 UI에 있는 QLineEdit으로부터 스트링 데이터를 얻어서 Manager클래스의 Map에 추가합니다.(상품의 경우 ID는 자동으로 생성합니다)
    QString name = ui.NameEdit->text();         //상품 이름 스트링 데이터를 QLineEdit으로부터 얻습니다
    QString price = ui.PriceEdit->text();       //상품 가격 데이터를 QLineEdit으로부터 얻습니다
    QString qty = ui.QuantityEdit->text();      //상품 구매 개수 데이터를 QLineEdit으로부터 얻습니다
    PView::addProduct(name,price,qty);          //Map에 상품을 추가합니다
}


AddProductView::~AddProductView(){

}


//Order================================================================================
AddOrderView::AddOrderView(Manager& mgr, Tree& tabs, const QIcon icon, const QString label) : OView{mgr,tabs,icon,label} {
    ui.setupUi(this);
    CPTab=ui.CPTab;             //고객과 상품을 선택할 수 있게 하는 QTabWidget
    infoTab=ui.infoTable;       //CPTab에서 마우스로 아이템을 선택하는 경우 그 아이템에 대한 정보를 보여주는 QTableWidget
    orderTree=ui.orderTree;     //구매자와 구매 물품을 선택하고 Commit버튼을 누르면 선택한 고객의 ID와 상품ID가 OrderTree로 이동합니다. 그 후 사용자는 구매할 수량을 결정할 수 있습니다
    QList<QString> oderTable_labels {tr("Client"),tr("Product Name"),tr("Qty")};    //주문
    orderTree->setColumnCount(oderTable_labels.size());     //열의 갯수를 위의 orderTable_labels에 있는 갯수만큼 설정
    orderTree->setHeaderLabels(oderTable_labels);
    shortcut = new QShortcut(Qt::Key_Delete, orderTree, orderTree, [this](){
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


    assert(connect(ui.commitOrderButton,SIGNAL(pressed()),this,SLOT(commitOrder())));
    assert(connect(ui.addOrderButton,SIGNAL(pressed()),this,SLOT(addOrder())));
    assert(connect(&clientTab,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged_())));
    assert(connect(&productTab,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged_())));

    ui.splitter_2->setSizes({ui.CPTab->sizeHint().width(),ui.orderTree->sizeHint().width()});
}


void AddOrderView::fillClientTab(){
    clientTab.clear();
    clientTab.setRowCount(mgr.getCM().getSize());
    clientTab.setColumnCount(3);
    clientTab.setHorizontalHeaderLabels({tr("Select"),tr("ID"),tr("Name")});

    int i=0;
    for(auto& client : mgr.getCM()){
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
    for(const auto& product : mgr.getPM()){
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
        fillClientTab();
        fillProductTab();
    }
    else{
    }
}

void AddOrderView::addOrder(){
    if(orderTree->topLevelItemCount()==0){
        QMessageBox::warning(this, tr("Warning"), tr("No order list was created\n Commit order first."));
    }
    for(int i=0; i<orderTree->topLevelItemCount(); i++){
        auto client = orderTree->topLevelItem(i);
        QString client_id = client->data(0, Role::id).value<QString>();
        std::vector<OrderModel::bill> products;
        for(int j=0; j<client->childCount(); j++){
            auto product = client->child(j);
            auto qty_box = static_cast<QSpinBox*>(orderTree->itemWidget(product,2));
            QString pid = product->data(0, Role::id).value<QString>();
            auto target_product = mgr.getPM().findProduct(pid.toStdString());
            if(target_product.getQty()<(unsigned int)qty_box->value()){
                QMessageBox::warning(this, tr("Warning"), tr("%1's total stock is less than %2").arg(QString(target_product.getName().c_str())).arg(qty_box->value()));
                return;
            }

            products.emplace_back(pid.toStdString(), qty_box->value());
        }
        bool result;
        std::tie(std::ignore,result)=OView::addOrder(client_id,std::move(products));
    }
    orderTree->clear();
}



AddOrderView::~AddOrderView(){
    delete shortcut;
}




AddParticipantView::AddParticipantView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label)
    : NView{mgr, tabs, icon,label} {
    initUI();

    fillContents();

    assert(connect(ui.addButton,SIGNAL(pressed()),this,SLOT(addParticipant())));
    assert(connect(ui.dropButton,SIGNAL(pressed()),this,SLOT(dropParticipant())));
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
    QList<QString> p_list_headers {tr("ID"),tr("Name")};
    ui.participantList->setColumnCount(p_list_headers.size());
    ui.participantList->setHorizontalHeaderLabels(p_list_headers);
}

void AddParticipantView::addParticipant(){
    auto ranges = ui.clientList->selectedRanges();
    if(ranges.empty()) return;
    for(auto range : ranges){
        for(int top = range.topRow(); top<= range.bottomRow(); top++){
            auto id_item = ui.clientList->item(top,0);
            QString id = id_item->data(Role::id).value<QString>();
            auto client = mgr.getCM().copyClient(id.toStdString());
            mgr.getSM().addClient(client);
        }
    }
    fillContents();
    notify<NView>();
}
void AddParticipantView::dropParticipant(){
    auto ranges = ui.participantList->selectedRanges();
    if(ranges.empty()) return;
    for(auto range : ranges){
        for(int top = range.topRow(); top<= range.bottomRow(); top++) {
            auto id_item = ui.participantList->item(top,0);
            QString id = id_item->data(Role::id).value<QString>();
            mgr.getSM().dropClient(id);
        }
    }
    fillContents();
    notify<NView>();
}
#include <memory>
void AddParticipantView::fillContents(){
    //고객 리스트로부터 채팅방 참여자를 결정할 수 있습니다.
    //ClientManager와 ServerManager 모두 고객 ID를 맵으로 저장하고 있기 때문에 고객 ID가 정렬되어 있습니다. 서로 정렬된 ID를 비교해서 채팅방 참여자인지 아닌지 O(n)의 시간 복잡도로 결정할 수 있습니다
    ui.clientList->clearContents();
    ui.clientList->setRowCount(mgr.getCM().getSize());

    int i=0;
    auto& cm = mgr.getCM();
    auto participants = mgr.getSM().begin();
    for(auto client_iter = cm.begin(); client_iter!=cm.end(); ++client_iter){   //두개 리스트 모두 고객 ID로 정렬된 상태에서 ClientManager가 갖고있는 고객 ID를 기준으로 채팅방 참여자가 아닌 고객 ID에 한하여 QListWidget에 그 ID로 추가합니다
        while(participants!=mgr.getSM().end() && client_iter!=cm.end() && ((*client_iter).getId()==participants->second.getClient().getId())){ //만약 고객 id와 참여자 id가 같다면 이는 고객을 보여주는 QListWidget에 아이템을 추가하지 않고 넘어가야 합니다(채팅방 참여자로 추가해야 하므로)
            ++client_iter;
            ++participants;
            continue;
        }
        if(client_iter == cm.end())
            break;
        int j=0;
        const Client client = *client_iter; //채팅 참여자리스트가 아닌 참여가능한 고객리스트에 추가할 고객 정보를 얻습니다.
        ui.clientList->setItem(i,j++,ceateTableItem(client.getId().c_str(), client.getId().c_str()) );  //ID정보를 갖는 셀을 생성합니다.
        ui.clientList->setItem(i,j++,new QTableWidgetItem(client.getName().c_str()));   //이름 정보를 갖는 셀을 생성합니다.
        i++;
    }
    ui.clientList->setRowCount(i);      //고객리스트 행을 설정합니다
    ui.clientList->resizeColumnsToContents();   //크기조절
    ui.clientList->resizeRowsToContents();      //크기조절

    ui.participantList->setRowCount((int)mgr.getSM().getSize());
    int p_i=0;
    for(auto& participant : mgr.getSM()){   //고객리스트는 이전에 이미 출력했으므로 채팅방 참여자 리스트만
        int j=0;
        auto& client = participant.second.getClient();
        ui.participantList->setItem(p_i,j++,ceateTableItem(client.getId().c_str(), client.getId().c_str()) );
        ui.participantList->setItem(p_i,j++,new QTableWidgetItem(client.getName().c_str()));
        p_i++;
    }
    ui.participantList->resizeColumnsToContents();
    ui.participantList->resizeRowsToContents();
}

AddParticipantView::~AddParticipantView(){}



void AddParticipantView::update(){
    fillContents();
}
