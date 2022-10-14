#include "ttest.h"
#include "ui_ttest.h"

ttest::ttest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ttest)
{
    ui->setupUi(this);
}

ttest::~ttest()
{
    delete ui;
}
