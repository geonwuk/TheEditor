#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <list>

#include <QMainWindow>

#include "tabwidget.h"

#include "Tree.h"
#include "MainManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;
             class dashboard;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    friend class MainManager;
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MainManager* getMgr(){return &mgrs;}
private slots:
    void save();
    void load();
    void onRadioButtonDBClicked();
    void onRadioMemoryButtonClicked();
private:
    Ui::MainWindow *ui;     //소멸자에서 지움
    Ui::dashboard* dash_board;      //소멸자에서 지움
    MainManager mgrs{*this};
    TabWidget management_tw{this};
    ManagementTree management_tree{this,&management_tw};
    TabWidget network_tw{this};
    NetworkTree network_tree{this,&network_tw};
    QStackedWidget* sw;     //ui가 자원을 지움
};
#endif // MAINWINDOW_H
