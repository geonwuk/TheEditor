#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

class MainWindow;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(MainWindow* parent=nullptr);
public slots:
    void tabClosed(int);
    void tabCurrentChanged(int index);
private:
    MainWindow* parent; //Main이 삭제
signals:
    void tabCurrentChanged_(int index);

};

#endif // TABWIDGET_H
