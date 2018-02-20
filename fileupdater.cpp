    #include "fileupdater.h"
#include <QFile>
#include <QFileInfo>

#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/copy.hpp>

FileUpdater::FileUpdater() {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handle_network_response(QNetworkReply*)));
}

FileUpdater::~FileUpdater() {
    delete networkManager;
}

void FileUpdater::download_files(std::vector<std::pair<std::string, std::string> > dl_filenames) {
    emit update_download_request(0, "Downloading files...");
    for (auto i : dl_filenames) {
        bz2_to_files[i.second] = i.first;
        download_file(i.second);
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
    QString filename = reply->request().url().fileName();

    reply->deleteLater();

    // this whole section below blocks usage of the application
    // need to move to a different thread to write and extract i guess?


    // ^ bad fix below, need to handle better...
    QCoreApplication::processEvents();

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    QCoreApplication::processEvents();
    // we probably don't even need to write to file, close, then reopen. TODO
    extract(filename.toStdString());
}

void FileUpdater::extract(std::string filename) {
    std::string filename_strip = bz2_to_files[filename];

    qDebug() << QString::fromStdString(filename_strip);

    // decompress bz2
    std::ifstream inStream(filename, std::ios_base::in | std::ios_base::binary);
    std::ofstream outStream(filename_strip, std::ios_base::out | std::ios_base::binary);
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
    out.push(boost::iostreams::bzip2_decompressor());
    out.push(outStream);
    boost::iostreams::copy(inStream, out);


    // delete bz2

    QFile file(QString::fromStdString(filename));
    file.remove();

    qDebug() << "finished decompressing " << QString::fromStdString(filename);

    std::string str = "Updated " + filename_strip;
    emit update_download_request(++file_downloaded_count/bz2_to_files.size() * 100, str);
    if (file_downloaded_count >= bz2_to_files.size()) {
        emit update_download_request(100.0, "All files done updating.");
    }
}
