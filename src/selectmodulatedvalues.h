#ifndef SELECTMODULATEDVALUES_H
#define SELECTMODULATEDVALUES_H

#include <QDialog>

namespace Ui {
class SelectModulatedValues;
}

class SelectModulatedValues : public QDialog
{
    Q_OBJECT

public:
    explicit SelectModulatedValues(QWidget *parent = 0);
    ~SelectModulatedValues();

    bool a();
    bool b();
    bool c();
    bool d();

private:
    Ui::SelectModulatedValues *ui;
};

#endif // SELECTMODULATEDVALUES_H
