#ifndef SHOWVIEW_H
#define SHOWVIEW_H
#include "View/view.h"
#include "ui_showClient.h"
#include "ui_showOrder.h"


class MainWindow;
class ShowClientView : public CView
{
    Q_OBJECT
public:
    explicit ShowClientView(Manager& mgr);
    ~ShowClientView();
    void update();

private:
    Ui::showClient ui {};
    QTableWidget* table;
    const int col =3;
    void fillContents();
};


class ShowProductView : public PView
{
    Q_OBJECT
public:
    explicit ShowProductView(Manager& mgr);
    ~ShowProductView();
    void update();
private:
    using UI_PRODUCT = Ui::showClient;
    UI_PRODUCT ui {};
    QTableWidget* table;
    void fillContents();
};

class ShowOrderView : public OView
{
    Q_OBJECT
public:
    explicit ShowOrderView(Manager& mgr);
    ~ShowOrderView();
    void update();
public slots:
    void orderItemSelectionChanged_();

private:
    Ui::showOrder ui {};
    QTableWidget *orderTable;
    QTableWidget *orderInfoTable;
    void fillContents();
};

#endif // SHOWVIEW_H
