#include "two_factor_dialog.h"
#include "ui_two_factor_dialog.h"
#include <QDebug>

two_factor_dialog::two_factor_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::two_factor_dialog)
{
    ui->setupUi(this);
    connect(ui->two_factor_button_cancel, SIGNAL(rejected()), this, SLOT(close()));
    connect(ui->two_factor_button_accept, SIGNAL(accepted()), this, SLOT(two_factor_submit()));
    p = (MainWindow *)parent;
}

two_factor_dialog::~two_factor_dialog()
{
    delete ui;
}

void two_factor_dialog::two_factor_submit() {
    p->two_factor_submit(ui->two_factor_input->text());
    this->close();
}
