#ifndef TWO_FACTOR_DIALOG_H
#define TWO_FACTOR_DIALOG_H

#include <QDialog>

namespace Ui {
class two_factor_dialog;
}

class two_factor_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit two_factor_dialog(QWidget *parent = 0);
    ~two_factor_dialog();

private slots:
    void two_factor_submit();
private:
    Ui::two_factor_dialog *ui;
};

#endif // TWO_FACTOR_DIALOG_H
