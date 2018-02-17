#ifndef TWO_FACTOR_DIALOG_H
#define TWO_FACTOR_DIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class two_factor_dialog;
}

class two_factor_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit two_factor_dialog(QWidget *parent = 0);
    ~two_factor_dialog();

signals:
    void reset_login_request();
    void reset_login_label_request(std::string str);
private slots:
    void two_factor_submit();
    void update_close();
private:
    Ui::two_factor_dialog *ui;
    MainWindow *p;
};

#endif // TWO_FACTOR_DIALOG_H
