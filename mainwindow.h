#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "Authenticator.h"
#include "webupdater.h"
#include "fileupdater.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void two_factor_submit(QString qstr);
    void load_news();
    void check_for_updates();
    ~MainWindow();

private slots:
    void open_two_factor_dialog();
    void handleLoginButton();

    void open_ttr_website();
    void open_toonhq_website();

    void update_news(bool res, std::string in);
    void update_login_status(std::string str);
    void reset_login();

    void download_files(std::vector<std::pair<std::string, std::string>> dl_filenames);
private:
    Ui::MainWindow *ui;
    Authenticator *auth;
    WebUpdater *webu;
    FileUpdater *fileu;
};

#endif // MAINWINDOW_H
