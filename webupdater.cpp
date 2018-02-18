#include "webupdater.h"
#include <QDebug>
#include <QFileInfo>

WebUpdater::WebUpdater(){
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handle_network_response(QNetworkReply*)));

}
WebUpdater::~WebUpdater(){
    delete networkManager;
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

void WebUpdater::update_manifest() {
    // manifest to check current files with
    QUrl manifestUrl = QUrl("https://s3.amazonaws.com/cdn.toontownrewritten.com/content/patchmanifest.txt");

    QNetworkRequest request(manifestUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    networkManager->get(request);
}

void WebUpdater::handle_network_response(QNetworkReply *reply) {
    if (reply->error()) {
        emit update_news_request(false, reply->errorString().toStdString());
    } else {
        QByteArray data = reply->readAll();
        QJsonObject json_object = QJsonDocument::fromJson(data).object();

        // TODO: there's probably a more elegant way of handling this-- need to figure out what the response is...
        if (json_object.contains("title")) {
            QString str = QString::fromStdString("<p>") + json_object["date"].toString() + QString::fromStdString("</p>") + json_object["body"].toString();
            emit update_news_request(true, str.toStdString());

        } else {
            qDebug() << "manifest size: " << json_object.size();

            for (QString filename : json_object.keys()) {
                QString hash = json_object[filename].toObject()["hash"].toString();


                QJsonArray only_array = json_object[filename].toObject()["only"].toArray();
                bool is_win_32 = only_array.contains(QJsonValue(QString::fromStdString("win32")));

                // is this going to be a windows only launcher?
                if (!is_win_32) {
                    qDebug() << filename << " is not required for win32. skipping...";
                    continue;
                }

                if (!file_up_to_date(filename.toStdString(), hash.toStdString())) {
                    // TODO: redownload the file, patches in the future i guess...
                    qDebug() << filename << " not up to date!";
                } else {
                    qDebug() << filename << " up to date.";
                }
            }
        }
    }
    reply->deleteLater();
}

bool WebUpdater::file_up_to_date(std::string path, std::string dl_hash) {
    QFileInfo file(QString::fromStdString(path));
    bool exists = file.exists() && file.isFile();

    if (!exists) return false;

    // waiting on figuring out what hash and compHash are!
    return dl_hash == get_bz2_hash(path);
}

std::string WebUpdater::get_bz2_hash(std::string path) {
    return "TODO";
}
