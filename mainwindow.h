#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Authenticator.h"
#include "webupdater.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void two_factor_submit(QString qstr);
    void load_web();
    ~MainWindow();

private slots:
    void open_two_factor_dialog();
    void handleLoginButton();

    void open_ttr_website();
    void open_toonhq_website();

private:
    Ui::MainWindow *ui;
    Authenticator *auth;
    WebUpdater *webu;
};

#endif // MAINWINDOW_H
