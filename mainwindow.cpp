#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "two_factor_dialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->login_button, SIGNAL (clicked()), this, SLOT (handleLoginButton()));
    connect(ui->username_input, SIGNAL (returnPressed()), this, SLOT (handleLoginButton()));
    connect(ui->password_input, SIGNAL (returnPressed()), this, SLOT (handleLoginButton()));
}

void MainWindow::handleLoginButton() {
    auth = new Authenticator(this);
    bool res = auth->login(ui->username_input->text().toStdString(), ui->password_input->text().toStdString());
    if (!res) {
        qDebug() << "empty username/password OR other error!!!";
    } else {
        qDebug() << "login request successfully sent. waiting on response...";
    }
}

void MainWindow::open_two_factor_dialog() {
    two_factor_dialog *tfg = new two_factor_dialog(this);
    tfg->show();
}

void MainWindow::two_factor_submit(QString qstr) {
    auth->two_factor(qstr.toStdString());

}

MainWindow::~MainWindow()
{
    delete ui;
    delete auth;
}
