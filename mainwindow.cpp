#include "mainwindow.h"
#include "authenticator.h"
#include "ui_mainwindow.h"
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
    Authenticator *auth = new Authenticator();
    bool res = auth->login(ui->username_input->text().toStdString(), ui->password_input->text().toStdString());
    if (!res) {
        qDebug() << "empty username/password OR other error!!!";
    } else {
        qDebug() << "login request successfully sent. waiting on response...";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
