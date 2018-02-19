#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QSslSocket>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // thanks stackoverflow
    a.setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                      "    border: 1px solid #999999;"
                                      "    background:white;"
                                      "    width:10px;    "
                                      "    margin: 0px 0px 0px 0px;"
                                      "}"
                                      "QScrollBar::handle:vertical {"
                                      "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                      "    stop: 0 rgb(99, 99, 99), stop: 0.5 rgb(99, 99, 99), stop:1 rgb(99, 99, 99));"
                                      "    min-height: 0px;"
                                      "}"
                                      "QScrollBar::add-line:vertical {"
                                      "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                      "    stop: 0 rgb(99, 99, 99), stop: 0.5 rgb(99, 99, 99),  stop:1rgb(99, 99, 99));"
                                      "    height: 0px;"
                                      "    subcontrol-position: bottom;"
                                      "    subcontrol-origin: margin;"
                                      "}"
                                      "QScrollBar::sub-line:vertical {"
                                      "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                      "    stop: 0  rgb(99, 99, 99), stop: 0.5 rgb(99, 99, 99),  stop:1 rgb(99, 99, 99));"
                                      "    height: 0 px;"
                                      "    subcontrol-position: top;"
                                      "    subcontrol-origin: margin;"
                                      "}"
                                      ));

    MainWindow w;
    w.show();
    qDebug()<<"SSL version use for build: "<<QSslSocket::sslLibraryBuildVersionString();
    qDebug()<<"SSL version use for run-time: "<<QSslSocket::sslLibraryVersionNumber();
    qDebug()<<QCoreApplication::libraryPaths();

    return a.exec();
}
