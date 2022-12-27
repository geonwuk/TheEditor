#include "showview.h"

#include <cassert>
#include <vector>
#include <fstream>

#include <QDate>
#include <QDateTimeEdit>
#include <QFileDialog>


#include <QShortcut>
#include <QMenu>
#include <QProgressDialog>

#include <QMessageBox>

using std::string;
using std::vector;

ShowClientView::ShowClientView(MainManager& mgr, Tree &tabs, const QIcon icon, const string label) : ClinetView{mgr, tabs,icon,label}, id_col{0}
{
    is_edit_mode=false;
    ui.setupUi(this);
    table=ui.tableWidget;
    editBox=ui.checkBox;
    searchLineEdit=ui.lineEdit;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);                              //읽기모드 활성화
    QList<QString> header_labels {tr("ID"),tr("Name"),tr("Phone Number"),tr("Address")};    //테이블의 헤더 필드 설정
    table->setColumnCount(header_labels.size());
    table->setHorizontalHeaderLabels(header_labels);
    fillContents();                                                                         //테이블 내용을 채운다
    assert(connect(ui.checkBox,&QCheckBox::stateChanged, this, [=](int status){  //읽기모드 수정모드 변환 시그널 연결
       is_edit_mode=status;
       using ET = QAbstractItemView::EditTrigger;
       is_edit_mode ? table->setEditTriggers(ET(ET::AllEditTriggers & ~ET::SelectedClicked &~ET::CurrentChanged)) : table->setEditTriggers(QAbstractItemView::NoEditTriggers);
       //
    }));
    assert(connect(table,SIGNAL(cellChanged(int,int)),SLOT(cellChanged(int,int))));
    assert(connect(searchLineEdit,SIGNAL(returnPressed()),SLOT(returnPressed())));          //엔터키를 신호 연결
    shortcut =new QShortcut(Qt::Key_Delete, table, table, [this](){                //항목을 삭제할 때
        if(!is_edit_mode) return;               //읽기모드이면 바로 리턴한다
        eraseClient(table->currentRow());       //행을 지우기
    });
}
ShowClientView::~ShowClientView(){
    assert(disconnect(this));
    assert(disconnect(table));
    assert(disconnect(searchLineEdit));
    delete shortcut;        //delete 단축키 삭제
}

void ShowClientView::fillContents(){
    table->blockSignals(true);          //시그널을 막는다
    table->clearContents();             //모든 항목을 지운다
    table->setRowCount(getSize());      //고객 수 만큼 행 크기를 설정한다
    int i=0;
    for(const auto& client : mgr.getClientModel()){  //모든 고객에 대해 반복
        int j=0;    //행의 위치를 결정
        table->setItem(i,j++,ceateTableItem(client.getId().c_str(), client.getId().c_str()));       //고객 ID
        table->setItem(i,j++,new QTableWidgetItem(client.getName().c_str()));                       //고객 이름
        table->setItem(i,j++,new QTableWidgetItem(client.getPhoneNumber().c_str()));                //고객 전화번호
        table->setItem(i,j++,new QTableWidgetItem(client.getAddress().c_str()));                    //고객 주소
        i++;
    }
    table->resizeColumnsToContents();       //열의 크기를 알맞게 조절
    table->resizeRowsToContents();          //행의 크기를 알맞게 조절
    table->blockSignals(false);
}

void ShowClientView::update(){
    fillContents();
}

void ShowClientView::cellChanged(int row, int col){
    if(col==0){
        table->blockSignals(true);
        QMessageBox::information(this, tr("Clinet Show"), tr("ID can't be changed")); //id는 바꿀 수 없음
        auto id_item = table->item(row,0);
        QString id = id_item->data(Role::id).value<QString>();      //id가 들어있는 열로부터 id값을 얻는다
        table->setItem(row,0,ceateTableItem(id.toStdString().c_str(),id.toStdString().c_str())); //수정된 ID를 다시 원복한다
        table->blockSignals(true);
    }
    if(is_edit_mode){                                               //읽기모드인 경우 아무것도 하지 않는다
        auto id_item = table->item(row,0);
        const string id = id_item->data(Role::id).value<QString>().toStdString();      //id가 들어있는 열로부터 id값을 얻는다
        vector<string> ls;
        int d = table->columnCount();
        for(int i=1; i<d; i++){ //id를 제외한 이름,전화번호,주소를 리스트에 추가한다
            ls.push_back(table->item(row, i)->text().toStdString());
        }
        table->blockSignals(true);
        modifyClient(id,ls);    //id를 제외한 나머지 필드 값으로 고객 정보를 수정한다
        table->blockSignals(false);
    }
}
void ShowClientView::returnPressed(){                       //엔터를 칠 경우 슬롯 함수(검색한 경우 엔터 입력)
    fillContents();                                         //정보를 다시 채운 후
    QString str = searchLineEdit->text();                   //사용자가 입력한 검색 텍스트를 저장한다
    QList<int> rows_to_delete;
    for(int row=0; row<table->rowCount(); row++){           //테이블의 모든 행에 대해
        QStringList ls;
        for(int col=0; col<table->columnCount(); col++){    //열에 사용자가 입력한 데이터가 있는지 확인한다
            ls<<table->item(row,col)->text();
        }
        for(const auto &e : ls){                            //열에 있는 모든 데이터에 대해
            if(e.contains(str)) continue;                   //만약 있다면 지우는 행 목록에 추가히지 않고 반복문을 진행한다.
        }
        rows_to_delete<<row;                                //사용자가 입력한 값이 열에 없으므로 그 행을 지워야 하므로 삭제 리스트에 추가한다
    }
    int compensation=0;
    for(auto row:rows_to_delete){
        table->removeRow(row-compensation);
        compensation++;
    }
}

bool ShowClientView::eraseClient(int row){
    auto item = table->item(row,id_col);
    const string id = item->data(Role::id).value<QString>().toStdString();
    bool result = ClinetView::eraseClient(id);
    if(!result) return false;
    table->removeRow(row);
    table->update();
    return result;
}





//Product_------------------------------------------------------------
ShowProductView::ShowProductView(MainManager& mgr, Tree &tabs, const QIcon icon, const string label) : ProductView{mgr, tabs,icon,label} {
    ui.setupUi(this);
    table=ui.tableWidget;
    editBox=ui.checkBox;
    searchLineEdit=ui.lineEdit;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QList<QString> labels{tr("ID"),tr("Name"),tr("Price"),tr("Quantity"),tr("Date")};
    table->setColumnCount(labels.size());
    table->setHorizontalHeaderLabels(labels);
    fillContents();
    assert(connect(ui.checkBox,&QCheckBox::stateChanged, this, [=](int status){
       is_edit_mode=status;
       using ET = QAbstractItemView::EditTrigger;
       is_edit_mode ? table->setEditTriggers(ET(ET::AllEditTriggers & ~ET::SelectedClicked &~ET::CurrentChanged)) : table->setEditTriggers(QAbstractItemView::NoEditTriggers);
       if(is_edit_mode){
           for(int i=0; i<table->rowCount(); i++){
               QDateTimeEdit* dt =qobject_cast<QDateTimeEdit*>(table->cellWidget(i,4));
               dt->setReadOnly(false);
           }
       }
       else{
           for(int i=0; i<table->rowCount(); i++){
               QDateTimeEdit* dt =qobject_cast<QDateTimeEdit*>(table->cellWidget(i,4));
               dt->setReadOnly(true);
           }
       }

    }));
    assert(connect(table,SIGNAL(cellChanged(int,int)),SLOT(cellChanged(int,int))));
    assert(connect(searchLineEdit,SIGNAL(returnPressed()),SLOT(returnPressed())));
    shortcut = new QShortcut(Qt::Key_Delete, table, table, [this](){
        if(!is_edit_mode) return;
        eraseProduct(table->currentRow());
    });
}
ShowProductView::~ShowProductView(){
    assert(disconnect(this));
    assert(disconnect(table));
    assert(disconnect(searchLineEdit));
    delete shortcut;
}

void ShowProductView::fillContents(){
    table->blockSignals(true);
    table->clearContents();
    table->setRowCount(getSize());
    int i=0;
    for(const auto& product : mgr.getProductModel()){
        int j =0;
        table->setItem(i,j++,ceateTableItem(product.getId().c_str(), product.getId().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(product.getName().c_str()));
        table->setItem(i,j++,new QTableWidgetItem(QString::number(product.getPrice())));
        table->setItem(i,j++,new QTableWidgetItem(QString::number(product.getQty())));
        auto tm = product.getDate();

        const QDate date {tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday};
        const QTime time {tm.tm_hour,tm.tm_min,tm.tm_sec};
        const QDateTime dateTime {date,time};
        QDateTimeEdit* dt = View::getDateTimeEditWidget(dateTime,this);
        assert(connect(dt,&QDateTimeEdit::dateTimeChanged, this, [=]{
            cellChanged(i,j);
        }));
        table->setCellWidget(i,j++,dt);
        i++;
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->blockSignals(false);
}

void ShowProductView::update(){
    if(!is_update){
        fillContents();
    }
    else{
    }
}

void ShowProductView::returnPressed(){
    fillContents();
    QString str = searchLineEdit->text();
    QList<int> rows_to_delete;
    for(int row=0; row<table->rowCount(); row++){
        QStringList ls;
        for(int col=0; col<table->columnCount()-1; col++){
            ls<<table->item(row,col)->text();
        }
        ls<<qobject_cast<QDateTimeEdit*>(table->cellWidget(row,4))->dateTime().toString("yyyy/MM/dd hh:mm:ss");
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

void ShowProductView::cellChanged(int row, int){
    if(is_edit_mode){
        auto id_item = table->item(row,0);
        const string id = id_item->data(Role::id).value<QString>().toStdString();

        vector<string> ls;
        for(int i=1; i<4; i++){
            ls.push_back(table->item(row, i)->text().toStdString());
        }
        QDateTimeEdit* dt = qobject_cast<QDateTimeEdit*>(table->cellWidget(row,4));
        assert(dt!=nullptr);
        ls.push_back(dt->dateTime().toString("MM/dd/yy hh:mm:ss").toStdString());
        //CM::Client client {0,ls[0],ls[1],ls[3]};
        table->blockSignals(true);
        modifyProduct(id,ls);
        table->blockSignals(false);
    }
}

bool ShowProductView::eraseProduct(int row){
    auto item = table->item(row,id_col);
    const string id = item->data(Role::id).value<QString>().toStdString();
    bool result = ProductView::eraseProduct(id);

    if(!result) return false;
    table->removeRow(row);
    table->update();
    return result;
}


//ORDER-------------------------------------------------------------------------------------------------------
ShowOrderView::ShowOrderView(MainManager& mgr, Tree &tabs, const QIcon icon, const string label) : OrderView{mgr, tabs,icon,label} {
    ui.setupUi(this);
    orderTable=ui.orderTable;
    orderInfoTable=ui.orderInfoTable;
    orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    orderInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QList<QString> ls {tr("Order ID"),tr("Client"),tr("Price"),tr("Date")};
    orderTable->setColumnCount(ls.size());
    orderTable->setHorizontalHeaderLabels(ls);
    fillContents();
    //label.append(tr("Show Order"));

    assert(connect(orderTable,SIGNAL(itemSelectionChanged()),this,SLOT(orderItemSelectionChanged_())));

    QList<QString> pr_cell_names {tr("Product Name"),tr("Unit Price"),tr("Quantity"),tr("Total Price")};
    orderInfoTable->setColumnCount(pr_cell_names.size());
    orderInfoTable->setHorizontalHeaderLabels(pr_cell_names);
}
ShowOrderView::~ShowOrderView(){
    assert(disconnect(orderTable));
}

void ShowOrderView::fillContents() {

    orderTable->clearContents();
    orderTable->setRowCount((int)getSize());

    int i=0;
    for(const auto& order : mgr.getOrderModel()){
        int j=0;
        orderTable->setItem(i,j++,ceateTableItem(QString::number(order.getID()).toStdString(), QString::number(order.getID()).toStdString()));
        auto client = order.getClient();
        orderTable->setItem(i,j++, new QTableWidgetItem(client.getName().c_str()));
        unsigned int price=0;
        for(const auto& ordered_product : order.getProducts()){
            auto product = ordered_product.product;
            price += product.getPrice()*ordered_product.qty;
        }
        orderTable->setItem(i,j++, new QTableWidgetItem(QString::number(price)));
        const auto tm = order.getDate();
        const QDate date {tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday};
        const QTime time {tm.tm_hour,tm.tm_min,tm.tm_sec};
        const QDateTime dateTime {date,time};
        orderTable->setItem(i,j++,new QTableWidgetItem(dateTime.toString("MM/dd/yy hh:mm:ss")));
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
    const auto& product_data = order.getProducts();
    orderInfoTable->setRowCount((int)product_data.size());
    int i=0;
    for(const auto& pd : product_data){
        int j=0;
        auto product = pd.product;
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(product.getName().c_str()));
        unsigned int price = product.getPrice();
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(QString::number(price)));
        unsigned int qty = pd.qty;
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(QString::number(qty)));
        unsigned int local_price = price*qty;
        orderInfoTable->setItem(i, j++, new QTableWidgetItem(QString::number(local_price)));
        i++;
    }
}

void ShowOrderView::update(){       //옵저버 패턴의 update
    if(!is_update){                 //현재 is_update를 쓰지 않지만 나중에 최적화를 위해서 남겨 둠
        fillContents();             //다시 화면을 채우는 작업
    }
    else{
    }
}
#include <thread>

//---------------------------------------------------


ShowChatView::ShowChatView(MainManager& mgr, Tree &tabs, const QIcon icon, const string label) : NetworkView{mgr, tabs,icon,label}, smgr{mgr.getServerManager()}, producer{broker} {
    ui.setupUi(this);
    log_thread=new LogThread(this);
    addCosumerButton= ui.addButton;
    deleteConsumerButton = ui.deleteButton;
    consumerTreeWidget = ui.consumerTreeWidget;

    assert(connect(addCosumerButton, SIGNAL(pressed()), SLOT(addConsumer())));
    assert(connect(deleteConsumerButton, SIGNAL(pressed()), SLOT(delteConsumer())));

    smgr.registerChatView(this);
    //ui.splitter->setSizes({120,500});
    //ui.clientTreeWidget->selectionMode()

    removeAction = new QAction(tr("&Kick out"));
    assert(connect(removeAction, &QAction::triggered, this, [&]{                         //Kick out 컨텍스트 메뉴를 클릭하면 강퇴하기
        for(auto& item : ui.clientTreeWidget->selectedItems()) {
            QString id = item->text(1);
            mgr.getServerManager().dropClient(id.toStdString());
        }
        notify<NetworkView>();
    }));
    menu = new QMenu;
    menu->addAction(removeAction);
    ui.clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    fillclientTree();
    log_thread->start();
    assert(connect(ui.savePushButton, SIGNAL(pressed()), SLOT(savePressed())));
}

ShowChatView::~ShowChatView(){
    assert(disconnect(removeAction));
    assert(disconnect(removeAction));

    mgr.getServerManager().unregisterChatView(this);
    log_thread->terminate();
    log_thread->deleteLater();

    delete progressDialog;
    progressDialog=nullptr;
    delete menu;
    menu=nullptr;
    delete removeAction;
    removeAction=nullptr;
}


void ShowChatView::savePressed(){
    QString filename = QFileDialog::getSaveFileName(this);
    std::ofstream out(filename.toStdString());
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    for(int i=0; i<ui.messageTreeWidget->topLevelItemCount(); i++){
        QTextStream out(&file);
        auto item = ui.messageTreeWidget->topLevelItem(i);
        out << item->text(0) << ", ";
        out << item->text(1) << ", ";
        out << item->text(2) << ", ";
        out << item->text(3) << ", ";
        out << item->text(4) << ", ";
        out << item->text(5) << "\n";
    }
    file.close();
}
void ShowChatView::addConsumer(){
    const std::string file_name = QFileDialog::getSaveFileName(this,tr("Save file"),"",tr("text (*.txt *.text )")).toStdString();
    if(file_name.empty()){
        return;
    }
    consumers.emplace_back(broker, file_name);
    update();
}
void ShowChatView::delteConsumer(){
    int i = consumerTreeWidget->currentIndex().row();
    qDebug()<<i;
    if(i==-1){
        QMessageBox::warning(this, tr("Can't delete"), tr("Please Select row to delte"));
        return;
    }
    auto itr = std::next(consumers.begin(),i);
    if(broker.isEmpty()){
        consumers.erase(itr);
    }
    else{
        QMessageBox::warning(this, tr("Can't delete"), tr("Please Try later"));
    }
    update();
}

void ShowChatView::clientLogin(){}

void ShowChatView::addLog(const ServerManager::ChatMessage& msg ){
    QTreeWidgetItem* item = new QTreeWidgetItem(ui.messageTreeWidget);
    item->setText(0, QString::fromStdString(msg.ip));
    item->setText(1, QString::fromStdString(msg.port));
    item->setText(2, QString::fromStdString(msg.id));
    item->setText(3, QString::fromStdString(msg.name));
    item->setText(4, QString::fromStdString(msg.message));
    item->setText(5, QString::fromStdString(msg.time));
    for(int i=0; i<ui.messageTreeWidget->columnCount(); i++){
        ui.messageTreeWidget->resizeColumnToContents(i);
    }
    log_thread->appendData(item);
    update();

    ServerManager::ChatMessage m = msg;
    std::thread t {&LogProducer::produce,&producer,m,consumers.size()};
    t.detach();                     //데이터 생성을 쓰레드로 한다
    for(auto& e : consumers){       //consume함수는 쓰레드를 호출하는 함수로 소비가 쓰레드로 이루어진다
        e.consume(producer.session);
    }
}

void ShowChatView::fillclientTree(){
    ui.clientTreeWidget->clear();
    ui.consumerTreeWidget->clear();
    for(auto& participant : smgr){
        auto client = participant.second.getClient();
        auto icon = participant.second.isOnline() ? QStyle::SP_DialogYesButton : QStyle::SP_DialogNoButton;
        auto online_string = participant.second.isOnline() ? "online" : "offline";
        auto item = new QTreeWidgetItem{ui.clientTreeWidget,{online_string,client.getId().c_str(),client.getName().c_str()}};
        item->setIcon(0,qApp->style()->standardIcon(icon));
    }
    for(int i=0; i<ui.clientTreeWidget->columnCount(); i++){
        ui.clientTreeWidget->resizeColumnToContents(i);
    }
    for(auto & e : consumers){
        new QTreeWidgetItem(consumerTreeWidget, {QString::fromStdString(e.file_name)});
    }
}

void ShowChatView::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos){
    //auto items = ui.clientTreeWidget->selectedItems();
    QPoint globalPos = ui.clientTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);                                      //컨텍스트 실행
}

void ShowChatView::update(){
    fillclientTree();
}
