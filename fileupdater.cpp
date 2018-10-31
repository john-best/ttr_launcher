#include "fileupdater.h"
#include <QFile>
#include <QFileInfo>

#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/copy.hpp>

/*
 * constructor
 */
FileUpdater::FileUpdater() {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handle_network_response(QNetworkReply*)));

    // yay threading
    FileUpdaterWorker *worker = new FileUpdaterWorker();
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));

    // so these metatypes don't exist in qt so we make them exist
    qRegisterMetaType<std::unordered_map<std::string,std::string>>("std::unordered_map<std::string,std::string>");
    connect(this, SIGNAL(send_file_to_worker(QByteArray, std::string, std::unordered_map<std::string, std::string>)),
            worker, SLOT(worker_handle_file(QByteArray, std::string, std::unordered_map<std::string, std::string>)));
    connect(worker, SIGNAL(worker_update_download_request(double,std::string)), this, SLOT(send_update_download_request_to_main(double,std::string)));

    // run the worker thread (which waits for a signal!)
    workerThread.start();
}

FileUpdater::~FileUpdater() {
    delete networkManager;
    workerThread.quit();
    workerThread.wait();
}

/*
 * calls the download file function on each file in the vector
 * also populates the map for bz2 filename to actual filename
 */
void FileUpdater::download_files(std::vector<std::pair<std::string, std::string>> dl_filenames) {
    emit update_download_request(0, "Downloading files...");
    for (auto i : dl_filenames) {
        bz2_to_files[i.second] = i.first;
        download_file(i.second);
    }
}

/*
 * downloads the file by making a network request
 */
void FileUpdater::download_file(std::string filename) {
    QString url = "https://s3.amazonaws.com/download.toontownrewritten.com/patches/" + QString::fromStdString(filename);
    QUrl downloadUrl = QUrl(url);

    QNetworkRequest request(downloadUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, 2);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    qDebug() << "starting download for: " << QString::fromStdString(filename);
    networkManager->get(request);
}

/*
 * the network response for the fileupdater is just the file (or error)
 * download the file and then send a signal to worker to unpack
 */
void FileUpdater::handle_network_response(QNetworkReply *reply) {
    if (reply->error()) {
       qDebug() << "Error downloading file: " << reply->request().url().fileName();
       qDebug() << reply->error();
       return;
    }

    qDebug() << "Downloaded file: " << reply->request().url().fileName();
    QByteArray data = reply->readAll();
    QString filename = reply->request().url().fileName();
    reply->deleteLater();

    emit send_file_to_worker(data, filename.toStdString(), bz2_to_files);
}

/*
 * update text in mainwindow
 */
void FileUpdater::send_update_download_request_to_main(double progress, std::string text) {
    emit update_download_request(progress, text);
}

/*
 * write the file to disk from memory... ideally we don't store anything in memory and just push to disk
 * but this works i guess. TTR is only like 500mb anyways so it's not that big of a deal
 * unless you're running a potato computer
 */
void FileUpdaterWorker::worker_handle_file(QByteArray data, std::string filename, std::unordered_map<std::string, std::string> bz2_to_files) {
   QFile file(QString::fromStdString(filename));
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    // we probably don't even need to write to file, close, then reopen. TODO
    extract(filename, bz2_to_files);
}

/*
 * extract the bz2 using the boost library
 * honestly we can probably use something that doesn't take 20 years to compile like boost
 * but oh well; not the worst thing to happen.
 */
void FileUpdaterWorker::extract(std::string filename, std::unordered_map<std::string, std::string> bz2_to_files) {
    std::string filename_strip = bz2_to_files[filename];

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
    emit worker_update_download_request(((double) ++file_completed_count)/bz2_to_files.size() * 100, str);
    if (file_completed_count >= bz2_to_files.size()) {
        emit worker_update_download_request(100.0, "All files done updating.");
    }
}
