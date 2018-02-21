#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "two_factor_dialog.h"
#include "resourcepacks.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->login_status_label->setWordWrap(true);
    ui->login_status_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    ui->login_status_label->update();
    ui->download_label->setAlignment(Qt::AlignCenter);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->setFixedSize(this->width(), this->height());

    MainWindowWorker *worker = new MainWindowWorker();
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));

    connect(this, SIGNAL(send_copy_to_worker(std::string)), worker, SLOT(handle_dir_copy(std::string)));
    connect(worker, SIGNAL(copy_finished()), this, SLOT(handle_worker_complete()));
    workerThread.start();

    connect(ui->login_button, SIGNAL (clicked()), this, SLOT (handle_login_button()));
    connect(ui->username_input, SIGNAL (returnPressed()), this, SLOT (handle_login_button()));
    connect(ui->password_input, SIGNAL (returnPressed()), this, SLOT (handle_login_button()));
    connect(ui->ttr_website_button, SIGNAL(clicked()), this, SLOT(open_ttr_website()));
    connect(ui->toonhq_website_button, SIGNAL(clicked()), this, SLOT(open_toonhq_website()));
    connect(ui->resourcepacks_button, SIGNAL(clicked()), this, SLOT(open_resource_packs()));

    auth = new Authenticator();
    connect(auth, SIGNAL (two_factor_request()), this, SLOT(open_two_factor_dialog()));
    connect(auth, SIGNAL(login_status_update_request(std::string)), this, SLOT(update_login_status(std::string)));
    connect(auth, SIGNAL(reset_login_request()), this, SLOT(reset_login()));
    connect(auth, SIGNAL(login_success()), this, SLOT(save_settings()));

    webu = new WebUpdater();
    connect(webu, SIGNAL(download_files_request(std::vector<std::pair<std::string, std::string>>)), this, SLOT(download_files(std::vector<std::pair<std::string, std::string>>)));
    connect(webu, SIGNAL(update_download_request(double, std::string)), this, SLOT(update_download_info(double, std::string)));

    fileu = new FileUpdater();
    connect(fileu, SIGNAL(update_download_request(double, std::string)), this, SLOT(update_download_info(double,std::string)));


    web_view = new QWebEngineView(ui->web_view_parent);
    web_view->setFixedSize(ui->web_view_parent->width(), ui->web_view_parent->height());

    read_settings();
    load_news();
    check_for_updates();
}

MainWindow::~MainWindow() {
    workerThread.quit();
    workerThread.wait();

    delete ui;
    delete auth;
    delete webu;
    delete fileu;
    delete web_view;
}

void MainWindow::handle_login_button() {
    ui->login_status_label->setText("");
    bool res = auth->login(ui->username_input->text().toStdString(), ui->password_input->text().toStdString());
    if (!res) {
        qDebug() << "empty username/password OR other error!!!";
        update_login_status("Empty username/password, or other error.");
    } else {
        qDebug() << "login request successfully sent. waiting on response...";
        update_login_status("Login request sent to server. Waiting...");
        disable_login();
    }
}

void MainWindow::open_two_factor_dialog() {
    // TODO memory leak here
    two_factor_dialog *tfg = new two_factor_dialog(this);
    connect(tfg, SIGNAL(reset_login_request()), this, SLOT(reset_login()));
    connect(tfg, SIGNAL(reset_login_label_request(std::string)), this, SLOT(update_login_status(std::string)));
    tfg->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);
    tfg->setFixedSize(tfg->width(), tfg->height());
    tfg->exec();
}

void MainWindow::two_factor_submit(QString qstr) {
    ui->login_status_label->setText("Two-factor submitted! Waiting on response...");
    auth->two_factor(qstr.toStdString());

}

void MainWindow::load_news() {
    web_view->load(QUrl("https://www.toontownrewritten.com/news"));
    web_view->show();
}

void MainWindow::check_for_updates() {
    ui->download_bar->show();
    ui->download_label->setText("Checking for updates...");
    ui->download_label->show();
    webu->update_manifest();
}

void MainWindow::update_download_info(double progress, std::string text) {

    if (progress == 100.0) {
        reset_login();
    } else {
        disable_login();
    }
    ui->download_bar->setValue(progress);
    ui->download_label->setText(QString::fromStdString(text));
}

void MainWindow::download_files(std::vector<std::pair<std::string, std::string> > dl_filenames) {
    if (dl_filenames.size() > 0) {
        qDebug() << "There are files to be updated... downloading now.";

        std::string str = dl_filenames.size() + " files need to be updated. Starting soon...";
        update_download_info(0, str);
        fileu->download_files(dl_filenames);
    } else {
        update_download_info(100.0, "All files are up to date.");
    }
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
    ui->resourcepacks_button->setEnabled(true);
}

void MainWindow::disable_login() {
    ui->username_input->setDisabled(true);
    ui->password_input->setDisabled(true);
    ui->login_button->setDisabled(true);
    ui->resourcepacks_button->setDisabled(true);
}

void MainWindow::read_settings() {
    QSettings settings("launcher_settings.ini", QSettings::IniFormat);
    settings.beginGroup("Launcher");

    if (settings.contains("username")) {
        ui->username_input->setText(settings.value("username", "").toString());
    }

    if (settings.contains("resource_pack_dir")) {
        current_resource_pack = settings.value("resource_pack_dir", "").toString().toStdString();
    }
}

void MainWindow::save_settings() {
    QSettings settings("launcher_settings.ini", QSettings::IniFormat);
    settings.beginGroup("Launcher");
    settings.setValue("username", ui->username_input->text());
    settings.setValue("resource_pack_dir", QString::fromStdString(current_resource_pack));
    settings.sync();
}

void MainWindow::open_resource_packs() {
    // TODO: memory leak here, probably.
    ResourcePacks *resource_packs_window = new ResourcePacks(0, current_resource_pack);
    connect(resource_packs_window, SIGNAL(send_pack_dir_to_main(std::string)), this, SLOT(set_resource_pack(std::string)));

    resource_packs_window->setModal(true);
    resource_packs_window->show();
}

void MainWindow::set_resource_pack(std::string resource_pack) {

    // should we check if they're currently equal? if so, do we still want to do something?
    current_resource_pack = resource_pack;

    QSettings settings("launcher_settings.ini", QSettings::IniFormat);
    settings.beginGroup("Launcher");
    settings.setValue("resource_pack_dir", QString::fromStdString(current_resource_pack));
    settings.sync();

    emit send_copy_to_worker(resource_pack);
    ui->download_label->setText("Applying resource pack changes...");
    disable_login();
}

void MainWindow::handle_worker_complete() {
    reset_login();
    ui->download_label->setText("Completed applying resource pack changes.");
}

void MainWindowWorker::handle_dir_copy(std::string path) {

    // delete everything so we copy every time (this is bad, but it's a solution)
    QDir dest("resources/");
    dest.removeRecursively();
    this->thread()->sleep(1);
    dest.mkpath(".");

    if (path == "") {
        emit copy_finished();
        return;
    }

    QDir pack("resource_packs/" + QString::fromStdString(path));

    if (!pack.exists() || pack.isEmpty()) {
        emit copy_finished();
        return;
    }

    QStringList pack_files = pack.entryList(QDir::Files | QDir::NoDotAndDotDot);
    foreach(QString pack_file, pack_files) {
        // yuck
        QFile::copy("resource_packs/" + QString::fromStdString(path) + "/" + pack_file, "resources/" + pack_file);
    }

    emit copy_finished();
}
