#include "webupdater.h"
#include <QDebug>
#include <QFileInfo>
#include <QMetaType>

WebUpdater::WebUpdater() {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handle_network_response(QNetworkReply*)));


    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::vector<std::pair<std::string,std::string>>>("std::vector<std::pair<std::string,std::string>>");
    WebUpdaterWorker *worker = new WebUpdaterWorker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(handle_manifest(QByteArray)), worker, SLOT(handle_manifest_worker(QByteArray)));
    connect(worker, SIGNAL(update_download_request_worker(double,std::string)), this, SLOT(handle_download_request(double,std::string)));
    connect(worker, SIGNAL(resultReady(std::vector<std::pair<std::string,std::string>>)), this, SLOT(handle_finished_manifest_request(std::vector<std::pair<std::string,std::string>>)));
    workerThread.start();
}

WebUpdater::~WebUpdater(){
    delete networkManager;
    workerThread.quit();
    workerThread.wait();
}

void WebUpdater::get_news() {
    // ttr news api
    // looks like we're getting the latest news for now until i figure out how to live my life
    QUrl apiUrl = QUrl("https://www.toontownrewritten.com/api/news");

    // set up request
    QNetworkRequest request(apiUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    // send request
    networkManager->get(request);
}

void WebUpdater::handle_network_response(QNetworkReply *reply) {
    if (reply->error()) {
        emit update_news_request(false, reply->errorString().toStdString());
    } else {
        QByteArray data = reply->readAll();
        QJsonObject json_object = QJsonDocument::fromJson(data).object();

        // "title" is only in news -- since we only update manifest and news, don't think we need to change much for now.
        if (json_object.contains("title")) {
            QString str = QString::fromStdString("<p>") + json_object["date"].toString() + QString::fromStdString("</p>") + json_object["body"].toString();
            emit update_news_request(true, str.toStdString());

        } else {
            handle_manifest(data);
        }
    }

    reply->deleteLater();
}

void WebUpdater::update_manifest() {
    // manifest to check current files with
    QUrl manifestUrl = QUrl("https://s3.amazonaws.com/cdn.toontownrewritten.com/content/patchmanifest.txt");

    QNetworkRequest request(manifestUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    networkManager->get(request);
}

void WebUpdater::handle_download_request(double progress, std::string text) {
    emit update_download_request(progress, text);
}

void WebUpdater::handle_finished_manifest_request(std::vector<std::pair<std::string, std::string> > filenames_pair) {
    emit download_files_request(filenames_pair);
}

// WebUpdateWorker methods
void WebUpdaterWorker::handle_manifest_worker(QByteArray data) {

    QJsonObject json_object = QJsonDocument::fromJson(data).object();
    // pair<filename, dl in bz2>
    std::vector<std::pair<std::string, std::string>> needs_dl;

    double size = json_object.size();
    double counter = 0.0;

    for (QString filename : json_object.keys()) {
        QString hash = json_object[filename].toObject()["hash"].toString();

        QJsonArray only_array = json_object[filename].toObject()["only"].toArray();
        bool is_win_32 = only_array.contains(QJsonValue(QString::fromStdString("win32")));

        // is this going to be a windows only launcher?
        if (!is_win_32) {
            qDebug() << filename << "is not required for win32. skipping...";
            continue;
        }

        if (!file_up_to_date(filename.toStdString(), hash.toStdString())) {
            std::string s = (filename + " not up to date.").toStdString();
            emit update_download_request_worker((counter/size) * 100, s);

            qDebug() << filename << "not up to date!";
            QString dl_filename = json_object[filename].toObject()["dl"].toString();
            std::pair<std::string, std::string> dl_pair(filename.toStdString(), dl_filename.toStdString());
            needs_dl.push_back(dl_pair);
        } else {
            std::string s = (filename + " up to date.").toStdString();
            emit update_download_request_worker((++counter/size) * 100, s);
            qDebug() << filename << "up to date.";
        }
    }

    emit resultReady(needs_dl);
}


bool WebUpdaterWorker::file_up_to_date(std::string path, std::string dl_hash) {
    QFileInfo file(QString::fromStdString(path));
    bool exists = file.exists() && file.isFile();

    if (!exists) return false;

    std::string file_hash = "";

    QFile f(QString::fromStdString(path));
    if(f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if (hash.addData(&f)) {
            file_hash = hash.result().toHex().toStdString();
        }
    }

    // bad fix, all updating stuff needs to run in a new thread or something
    //QCoreApplication::processEvents();
    return dl_hash == file_hash;
}
