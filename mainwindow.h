#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Authenticator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void open_two_factor_dialog();
    void two_factor_submit(QString qstr);
    ~MainWindow();

private slots:
    void handleLoginButton();
private:
    Ui::MainWindow *ui;
    Authenticator *auth;
};

#endif // MAINWINDOW_H
