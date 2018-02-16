#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QSslSocket>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.load_news();
    w.check_for_updates();

    qDebug()<<"SSL version use for build: "<<QSslSocket::sslLibraryBuildVersionString();
    qDebug()<<"SSL version use for run-time: "<<QSslSocket::sslLibraryVersionNumber();
    qDebug()<<QCoreApplication::libraryPaths();

    return a.exec();
}
