#include "two_factor_dialog.h"
#include "ui_two_factor_dialog.h"
#include <QDebug>

two_factor_dialog::two_factor_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::two_factor_dialog)
{
    ui->setupUi(this);
    connect(ui->two_factor_button_cancel, SIGNAL(rejected()), this, SLOT(update_close()));
    connect(ui->two_factor_button_accept, SIGNAL(accepted()), this, SLOT(two_factor_submit()));
    p = (MainWindow *)parent;
}

/*
 * destructor
 */
two_factor_dialog::~two_factor_dialog() {
    delete ui;
}

/*
 * submit two factor
 */
void two_factor_dialog::two_factor_submit() {
    p->two_factor_submit(ui->two_factor_input->text());
    QDialog::accept();
}

/*
 * close two factor
 */
void two_factor_dialog::update_close() {
    emit reset_login_request();
    emit reset_login_label_request("");
    this->close();
}

/*
 * reject two factor (close but red x/cancel)
 */
void two_factor_dialog::reject() {
    emit reset_login_request();
    emit reset_login_label_request("");
    QDialog::reject();
}
