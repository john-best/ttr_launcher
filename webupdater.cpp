#include "webupdater.h"
#include <QDebug>

WebUpdater::WebUpdater(){
    networkManager = new QNetworkAccessManager(this);
}
WebUpdater::~WebUpdater(){
    delete networkManager;
}

void WebUpdater::load_news() {
    // ttr news api
    // looks like we're getting the latest news for now until i figure out how to live my life
    QUrl apiUrl = QUrl("https://www.toontownrewritten.com/api/news");

    // set up request
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    // send request
    networkManager->get(request);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(handle_network_response(QNetworkReply*)));
}

void WebUpdater::handle_network_response(QNetworkReply *reply) {
    if (reply->error()) {
        emit update_news_request(false, reply->errorString().toStdString());
        return;
    }

    QByteArray data = reply->readAll();
    QJsonObject json_object = QJsonDocument::fromJson(data).object();
    reply->deleteLater();

    emit update_news_request(true, json_object["body"].toString().toStdString());
}
