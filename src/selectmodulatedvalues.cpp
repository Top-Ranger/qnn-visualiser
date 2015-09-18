#include "selectmodulatedvalues.h"
#include "ui_selectmodulatedvalues.h"

SelectModulatedValues::SelectModulatedValues(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectModulatedValues)
{
    ui->setupUi(this);
}

SelectModulatedValues::~SelectModulatedValues()
{
    delete ui;
}

bool SelectModulatedValues::a()
{
    return ui->checkBox_A->isChecked();
}

bool SelectModulatedValues::b()
{
    return ui->checkBox_B->isChecked();
}

bool SelectModulatedValues::c()
{
    return ui->checkBox_C->isChecked();
}

bool SelectModulatedValues::d()
{
    return ui->checkBox_D->isChecked();
}
