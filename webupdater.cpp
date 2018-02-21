#include "webupdater.h"
#include <QDebug>
#include <QFileInfo>

// constructor
WebUpdater::WebUpdater() {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handle_network_response(QNetworkReply*)));

    // register meta types so they can be used in connect()
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::vector<std::pair<std::string,std::string>>>("std::vector<std::pair<std::string,std::string>>");

    // somehow this works, i mostly understand how it works but i'm still a little hopeless
    WebUpdaterWorker *worker = new WebUpdaterWorker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(send_manifest_to_worker(QByteArray)), worker, SLOT(worker_handle_manifest(QByteArray)));

    // pass request to request
    connect(worker, SIGNAL(worker_update_download_request(double,std::string)), this, SLOT(send_update_download_request_to_main(double,std::string)));
    connect(worker, SIGNAL(worker_required_files_request(std::vector<std::pair<std::string,std::string>>)), this, SLOT(send_required_files_to_main(std::vector<std::pair<std::string,std::string>>)));
    workerThread.start();
}

// destructor
WebUpdater::~WebUpdater(){
    delete networkManager;
    workerThread.quit();
    workerThread.wait();
}

/* get_news()
 * send web request to get the news
 */
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

/*
 * handle_network_response(reply)
 * 1) news response, then send text to main window immediately because it's not that blocking
 * 2) manifest response, then send to worker because it takes too long
 */
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
            send_manifest_to_worker(data);
        }
    }

    reply->deleteLater();
}

/*
 * update_manifest()
 * send web request to get manifest
 */
void WebUpdater::update_manifest() {
    // manifest to check current files with
    QUrl manifestUrl = QUrl("https://s3.amazonaws.com/cdn.toontownrewritten.com/content/patchmanifest.txt");

    QNetworkRequest request(manifestUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    networkManager->get(request);
}

/*
 * send_update_download_request_to_main(progress, text)
 * pass progress bar stuff to main to show changes to the user from the gui
 * note: this function name is awful send help
 */
void WebUpdater::send_update_download_request_to_main(double progress, std::string text) {
    emit update_download_request(progress, text);
}

/*
 * send_required_files_to_main(filenames_pair)
 * pass files we need to download to main to send to fileupdater
 * note: this function name is awful send help
 */
void WebUpdater::send_required_files_to_main(std::vector<std::pair<std::string, std::string> > filenames_pair) {
    emit download_files_request(filenames_pair);
}


// WebUpdateWorker methods
/*
 * worker_handle_manifest(data)
 * generate files needed to download from the manifest given
 */
void WebUpdaterWorker::worker_handle_manifest(QByteArray data) {

    QJsonObject json_object = QJsonDocument::fromJson(data).object();

    // pair<filename, dl in bz2>
    std::vector<std::pair<std::string, std::string>> needs_dl;

    double size = json_object.size();
    double counter = 0.0;

    // loop through all files in the json
    for (QString filename : json_object.keys()) {

        // SHA1 hash of the file being checked
        QString hash = json_object[filename].toObject()["hash"].toString();

        // win32 launcher only for now, so we only need to check for win32 files
        QJsonArray only_array = json_object[filename].toObject()["only"].toArray();
        bool is_win_32 = only_array.contains(QJsonValue(QString::fromStdString("win32")));

        if (!is_win_32) {
            qDebug() << filename << "is not required for win32. skipping...";
            continue;
        }

        // check for file up to date
        if (!file_up_to_date(filename.toStdString(), hash.toStdString())) {
            std::string s = (filename + " not up to date.").toStdString();
            emit worker_update_download_request((counter/size) * 100, s);

            qDebug() << filename << "not up to date!";

            // grab download link
            QString dl_filename = json_object[filename].toObject()["dl"].toString();

            // pair of filename and download filename (which is a bz2)
            std::pair<std::string, std::string> dl_pair(filename.toStdString(), dl_filename.toStdString());

            needs_dl.push_back(dl_pair);

        } else {

            // the file is up to date so we don't have to do anything.
            std::string s = (filename + " up to date.").toStdString();
            emit worker_update_download_request((++counter/size) * 100, s);
            qDebug() << filename << "up to date.";
        }
    }

    // we're done here, let's pass this over to fileupdater
    emit worker_required_files_request(needs_dl);
}


/*
 * file_up_to_date(path, dl_hash)
 * check if file is updated by checking its hash against the download's decompressed hash
 */
bool WebUpdaterWorker::file_up_to_date(std::string path, std::string dl_hash) {
    QFileInfo file(QString::fromStdString(path));

    // exit early if we the file doesn't exist
    return file.exists() && file.isFile();

    // check hash
    std::string file_hash = "";
    QFile f(QString::fromStdString(path));
    if(f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if (hash.addData(&f)) {
            file_hash = hash.result().toHex().toStdString();
        }
    }

    // return boolean if download hash equals file hash
    return dl_hash == file_hash;
}
