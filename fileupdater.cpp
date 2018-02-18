#include "fileupdater.h"
#include <QFile>
#include <QFileInfo>


FileUpdater::FileUpdater() {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handle_network_response(QNetworkReply*)));
}

FileUpdater::~FileUpdater() {
    delete networkManager;
}

void FileUpdater::download_files(QStringList filenames) {
    for (QString filename : filenames) {
        download_file(filename.toStdString());
    }
}

void FileUpdater::download_file(std::string filename) {
    QString url = "https://download.toontownrewritten.com/content/patches/" + QString::fromStdString(filename);
    QUrl downloadUrl = QUrl(url);

    QNetworkRequest request(downloadUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, 2);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    qDebug() << "starting download for: " << QString::fromStdString(filename);
    networkManager->get(request);
}

void FileUpdater::handle_network_response(QNetworkReply *reply) {
    if (reply->error()) {
       qDebug() << "Error downloading file: " << reply->request().url().fileName();
       return;
    }

    qDebug() << "Downloaded file: " << reply->request().url().fileName();

    QByteArray data = reply->readAll();

    qDebug() << "size:" << data.size();
    reply->deleteLater();

    // we probably need to save the file, then open it again and extract using
    // the boost libraries because apparently qt doesn't have any de/compressions
    // much less for .bz2 files. TODO
}
