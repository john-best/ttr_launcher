#include "webupdater.h"

WebUpdater::WebUpdater(){
    networkManager = new QNetworkAccessManager(this);
}
WebUpdater::~WebUpdater(){
    delete networkManager;
}

void WebUpdater::load_news() {
    // ttr news api
    // TODO: apparently i can pass in a length to only get the latest x amount
    // but no documentation from devs so it's just guess and check until i find it??
    QUrl apiUrl = QUrl("https://www.toontownrewritten.com/api/news/list");

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

    // TODO: fancy html stuff i guess, but still need to check the result from reply first.
    emit update_news_request(true, "TTR website accessed! TODO");
}
