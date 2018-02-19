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
                    qDebug() << filename << "not up to date!";
                    QString dl_filename = json_object[filename].toObject()["dl"].toString();
                    std::pair<std::string, std::string> dl_pair(filename.toStdString(), dl_filename.toStdString());
                    needs_dl.push_back(dl_pair);

                } else {
                    std::string s = (filename + " up to date.").toStdString();

                    // ok this probably isn't the best way to do this but it works for now.
                    // TODO: what do we do when the files aren't up to date?
                    emit update_download_request((++counter/size) * 100, s);
                    qDebug() << filename << "up to date.";
                }
            }

            emit download_files_request(needs_dl);
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

bool WebUpdater::file_up_to_date(std::string path, std::string dl_hash) {
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
    QCoreApplication::processEvents();
    return dl_hash == file_hash;
}
