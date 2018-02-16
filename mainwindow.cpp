#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "two_factor_dialog.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->download_bar->hide();
    ui->download_label->hide();

    connect(ui->login_button, SIGNAL (clicked()), this, SLOT (handleLoginButton()));
    connect(ui->username_input, SIGNAL (returnPressed()), this, SLOT (handleLoginButton()));
    connect(ui->password_input, SIGNAL (returnPressed()), this, SLOT (handleLoginButton()));
    connect(ui->ttr_website_button, SIGNAL(clicked()), this, SLOT(open_ttr_website()));
    connect(ui->toonhq_website_button, SIGNAL(clicked()), this, SLOT(open_toonhq_website()));

    auth = new Authenticator();
    connect(auth, SIGNAL (two_factor_request()), this, SLOT(open_two_factor_dialog()));

    webu = new WebUpdater();
    connect(webu, SIGNAL(update_news_request(bool, std::string)), this, SLOT(update_news(bool, std::string)));
}

void MainWindow::handleLoginButton() {
    bool res = auth->login(ui->username_input->text().toStdString(), ui->password_input->text().toStdString());
    if (!res) {
        qDebug() << "empty username/password OR other error!!!";
    } else {
        qDebug() << "login request successfully sent. waiting on response...";
    }
}

void MainWindow::open_two_factor_dialog() {
    two_factor_dialog *tfg = new two_factor_dialog(this);
    tfg->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    tfg->show();
}

void MainWindow::two_factor_submit(QString qstr) {
    auth->two_factor(qstr.toStdString());

}

void MainWindow::load_news() {
    webu->load_news();
    ui->textBrowser->setText("Loading...");
}

void MainWindow::update_news(bool res, std::string in) {
    if (res == false) {
        ui->textBrowser->setText("Unable to load TTR Website!\n" + QString::fromStdString(in));
        return;
    }
    ui->textBrowser->setText(QString::fromStdString(in));
}

void MainWindow::check_for_updates() {}

MainWindow::~MainWindow()
{
    delete ui;
    delete auth;
    delete webu;
}

void MainWindow::open_ttr_website() {
    QDesktopServices::openUrl(QUrl("https://toontownrewritten.com"));
}

void MainWindow::open_toonhq_website() {
    QDesktopServices::openUrl(QUrl("https://toonhq.org"));
}
