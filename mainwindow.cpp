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
    ui->login_status_label->setWordWrap(true);
    ui->login_status_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    ui->login_status_label->update();
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->setFixedSize(this->width(), this->height());

    connect(ui->login_button, SIGNAL (clicked()), this, SLOT (handleLoginButton()));
    connect(ui->username_input, SIGNAL (returnPressed()), this, SLOT (handleLoginButton()));
    connect(ui->password_input, SIGNAL (returnPressed()), this, SLOT (handleLoginButton()));
    connect(ui->ttr_website_button, SIGNAL(clicked()), this, SLOT(open_ttr_website()));
    connect(ui->toonhq_website_button, SIGNAL(clicked()), this, SLOT(open_toonhq_website()));

    auth = new Authenticator();
    connect(auth, SIGNAL (two_factor_request()), this, SLOT(open_two_factor_dialog()));
    connect(auth, SIGNAL(login_status_update_request(std::string)), this, SLOT(update_login_status(std::string)));
    connect(auth, SIGNAL(reset_login_request()), this, SLOT(reset_login()));

    webu = new WebUpdater();
    connect(webu, SIGNAL(update_news_request(bool, std::string)), this, SLOT(update_news(bool, std::string)));
    connect(webu, SIGNAL(download_files_request(QStringList)), this, SLOT(download_files(QStringList)));

    fileu = new FileUpdater();

    load_news();
    check_for_updates();
}

void MainWindow::handleLoginButton() {
    ui->login_status_label->setText("");
    bool res = auth->login(ui->username_input->text().toStdString(), ui->password_input->text().toStdString());
    if (!res) {
        qDebug() << "empty username/password OR other error!!!";
        update_login_status("Empty username/password, or other error.");
    } else {
        qDebug() << "login request successfully sent. waiting on response...";
        update_login_status("Login request sent to server. Waiting...");
        ui->username_input->setDisabled(true);
        ui->password_input->setDisabled(true);
        ui->login_button->setDisabled(true);
    }
}

void MainWindow::open_two_factor_dialog() {
    // TODO memory leak here
    two_factor_dialog *tfg = new two_factor_dialog(this);
    connect(tfg, SIGNAL(reset_login_request()), this, SLOT(reset_login()));
    connect(tfg, SIGNAL(reset_login_label_request(std::string)), this, SLOT(update_login_status(std::string)));
    tfg->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMaximizeButtonHint);
    tfg->setFixedSize(tfg->width(), tfg->height());
    tfg->show();
}

void MainWindow::two_factor_submit(QString qstr) {
    ui->login_status_label->setText("Two-factor submitted! Waiting on response...");
    auth->two_factor(qstr.toStdString());

}

void MainWindow::load_news() {
    webu->get_news();
    ui->textBrowser->setText("Loading...");
}

void MainWindow::update_news(bool res, std::string in) {
    if (res == false) {
        ui->textBrowser->setText("Unable to load TTR Website!\n" + QString::fromStdString(in));
        return;
    }
    ui->textBrowser->setHtml(QString::fromStdString(in));
}

void MainWindow::check_for_updates() {
    webu->update_manifest();
}

void MainWindow::download_files(QStringList filenames) {
    qDebug() << "There are files to be updated... downloading now.";
    fileu->download_files(filenames);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete auth;
    delete webu;
    delete fileu;
}

void MainWindow::open_ttr_website() {
    QDesktopServices::openUrl(QUrl("https://toontownrewritten.com"));
}

void MainWindow::open_toonhq_website() {
    QDesktopServices::openUrl(QUrl("https://toonhq.org"));
}

void MainWindow::update_login_status(std::string str) {
    ui->login_status_label->setText(QString::fromStdString(str));
}

void MainWindow::reset_login() {
    ui->username_input->setEnabled(true);
    ui->password_input->setEnabled(true);
    ui->login_button->setEnabled(true);
}
