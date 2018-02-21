#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "Authenticator.h"
#include "webupdater.h"
#include "fileupdater.h"
#include <QWebEngineView>

namespace Ui {
class MainWindow;
}

class MainWindowWorker : public QObject {
    Q_OBJECT
    QThread workerThread;

signals:
    void copy_finished();

public slots:
    void handle_dir_copy(std::string path);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;

public:
    explicit MainWindow(QWidget *parent = 0);

    void two_factor_submit(QString qstr);
    void load_news();
    void check_for_updates();
    void read_settings();
    ~MainWindow();

signals:
    void send_copy_to_worker(std::string path);
private slots:
    void open_two_factor_dialog();
    void handle_login_button();

    void open_ttr_website();
    void open_toonhq_website();

    void update_login_status(std::string str);
    void reset_login();
    void disable_login();

    void update_download_info(double progress, std::string text);

    void download_files(std::vector<std::pair<std::string, std::string>> dl_filenames);

    void save_settings();

    void open_resource_packs();
    void set_resource_pack(std::string resource_pack);
    void handle_worker_complete();

private:
    Ui::MainWindow *ui;
    Authenticator *auth;
    WebUpdater *webu;
    FileUpdater *fileu;
    std::string current_resource_pack = "";
    QWebEngineView *web_view;

};

#endif // MAINWINDOW_H
