#ifndef TTEST_H
#define TTEST_H

#include <QWidget>

namespace Ui {
class ttest;
}

class ttest : public QWidget
{
    Q_OBJECT

public:
    explicit ttest(QWidget *parent = nullptr);
    ~ttest();

private:
    Ui::ttest *ui;
};

#endif // TTEST_H
