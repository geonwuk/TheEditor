#include "showview.h"
#include <QDate>
#include <cassert>
#include <QShortcut>
#include<QMenu>
#include <QProgressDialog>
#include <QDateTimeEdit>
#include <QFileDialog>
#include "Manager/ClientManager.h"
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
       using ET = QAbstractItemView::EditTrigger;
       is_edit_mode ? table->setEditTriggers(ET(ET::AllEditTriggers & ~ET::SelectedClicked &~ET::CurrentChanged)) : table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    });
    connect(table,SIGNAL(cellChanged(int,int)),SLOT(cellChanged(int,int)));
    connect(searchLineEdit,SIGNAL(returnPressed()),SLOT(returnPressed()));          //엔터키를 신호 연결
    shortcut = new QShortcut(Qt::Key_Delete, table, table, [this](){                //항목을 삭제할 때
        if(!is_edit_mode) return;
        eraseClient(table->currentRow());
    });
}
ShowClientView::~ShowClientView(){
    delete shortcut;
}

void ShowClientView::fillContents(){
    table->blockSignals(true);
    table->clearContents();
    table->setRowCount(getSize());
    int i=0;

//    for(IteratorElem<CM::Client>& b = getClients().begin(); b!=getClients().end(); ++b){
    for(auto client : getClients()){
        //auto client = *b;
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
}
void ShowClientView::cellChanged(int row, int col){
    if(is_edit_mode){
        auto id_item = table->item(row,0);
        QString id = id_item->data(Role::id).value<QString>();

        QList<QString> ls;
        int d = table->columnCount();
        for(int i=1; i<d; i++){
            ls<<table->item(row, i)->text();
        }
        table->blockSignals(true);
        modifyClient(id,ls);
        table->blockSignals(false);
    }
}
void ShowClientView::returnPressed(){
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
    if(!result) return false;
    table->removeRow(row);
    table->update();
    return result;
}





//Product_------------------------------------------------------------
ShowProductView::ShowProductView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label) : PView{mgr, tabs,icon,label} {
    ui.setupUi(this);
    table=ui.tableWidget;
    editBox=ui.checkBox;
    searchLineEdit=ui.lineEdit;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QList<QString> labels{tr("ID"),tr("Name"),tr("Price"),tr("Quantity"),tr("Date")};
    table->setColumnCount(labels.size());
    table->setHorizontalHeaderLabels(labels);
    fillContents();
    connect(ui.checkBox,&QCheckBox::stateChanged, [=](int status){
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

    });
    connect(table,SIGNAL(cellChanged(int,int)),SLOT(cellChanged(int,int)));
    connect(searchLineEdit,SIGNAL(returnPressed()),SLOT(returnPressed()));
    auto shortcut = new QShortcut(Qt::Key_Delete, table, table, [this](){
        if(!is_edit_mode) return;
        eraseProduct(table->currentRow());
    });

}
void ShowProductView::fillContents(){
    table->blockSignals(true);
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

        QDate date {tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday};
        QTime time {tm.tm_hour,tm.tm_min,tm.tm_sec};
        QDateTime dateTime {date,time};
        QDateTimeEdit* dt = View::getDateTimeEditWidget(dateTime,this);
        connect(dt,&QDateTimeEdit::dateTimeChanged,[=]{
            cellChanged(i,j);
        });
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
void ShowProductView::cellChanged(int row, int col){
    if(is_edit_mode){
        auto id_item = table->item(row,0);
        QString id = id_item->data(Role::id).value<QString>();

        QList<QString> ls;
        for(int i=1; i<4; i++){
            ls<<table->item(row, i)->text();
        }
        QDateTimeEdit* dt = qobject_cast<QDateTimeEdit*>(table->cellWidget(row,4));
        assert(dt!=nullptr);
        ls<<dt->dateTime().toString("MM/dd/yy hh:mm:ss");
        //CM::Client client {0,ls[0],ls[1],ls[3]};
        table->blockSignals(true);
        modifyProduct(id,ls);
        table->blockSignals(false);
    }
}

bool ShowProductView::eraseProduct(int row){
    auto item = table->item(row,id_col);
    QString id = item->data(Role::id).value<QString>();
    bool result = PView::eraseProduct(id);

    if(!result) return false;
    table->removeRow(row);
    table->update();
    return result;
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
        QDate date {tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday};
        QTime time {tm.tm_hour,tm.tm_min,tm.tm_sec};
        QDateTime dateTime {date,time};
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
    assert(order!=OM::no_order);
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

ShowOrderView::~ShowOrderView(){}
//---------------------------------------------------


ShowChatView::ShowChatView(Manager& mgr, Tree &tabs, const QIcon icon, const QString label) : NView{mgr, tabs,icon,label}, smgr{mgr.getSM()} {
    ui.setupUi(this);
    log_thread=new LogThread(this);

    smgr.registerChatView(this);
    //ui.splitter->setSizes({120,500});
    //ui.clientTreeWidget->selectionMode()

    QAction* removeAction = new QAction(tr("&Kick out"));
    connect(removeAction, &QAction::triggered, [&]{                         //Kick out 컨텍스트 메뉴를 클릭하면 강퇴하기
        for(auto& item : ui.clientTreeWidget->selectedItems()) {
            QString id = item->text(1);
            mgr.getSM().dropClient(id);
        }
        notify<NView>();
    });
    menu = new QMenu;
    menu->addAction(removeAction);
    ui.clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    fillclientTree();
    log_thread->start();
    connect(ui.savePushButton, SIGNAL(pressed()), SLOT(savePressed()));
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


ShowChatView::~ShowChatView(){
    mgr.getSM().unregisterChatView(this);
    log_thread->terminate();
}

void ShowChatView::clientLogin(){}

void ShowChatView::addLog(const ServerManager::ChatMessage& msg ){
    QTreeWidgetItem* item = new QTreeWidgetItem(ui.messageTreeWidget);
    item->setText(0, msg.ip);
    item->setText(1, msg.port);
    item->setText(2, msg.id);
    item->setText(3, msg.name);
    item->setText(4, msg.message);
    item->setText(5, msg.time);
    for(int i=0; i<ui.messageTreeWidget->columnCount(); i++){
        ui.messageTreeWidget->resizeColumnToContents(i);
    }
    log_thread->appendData(item);
}

void ShowChatView::fillclientTree(){
    ui.clientTreeWidget->clear();
    for(auto participant : smgr){
        int j=0;
        auto client = participant.second->getClient();
        auto icon = participant.second->isOnline() ? QStyle::SP_DialogYesButton : QStyle::SP_DialogNoButton;
        auto online_string = participant.second->isOnline() ? "online" : "offline";
        auto item = new QTreeWidgetItem{ui.clientTreeWidget,{online_string,client.getId().c_str(),client.getName().c_str()}};
        item->setIcon(0,qApp->style()->standardIcon(icon));
    }
    for(int i=0; i<ui.clientTreeWidget->columnCount(); i++){
        ui.clientTreeWidget->resizeColumnToContents(i);
    }
}

void ShowChatView::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos){
    auto items = ui.clientTreeWidget->selectedItems();
    QPoint globalPos = ui.clientTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);                                      //컨텍스트 실행
}

void ShowChatView::update(){
    fillclientTree();
}
