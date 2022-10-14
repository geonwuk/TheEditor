#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>

namespace Ui {
class NetWork;
}

class NetWork : public QWidget
{
    Q_OBJECT

public:
    explicit NetWork(QWidget *parent = nullptr);
    ~NetWork();

private:
    Ui::NetWork *ui;
};

#endif // NETWORK_H
