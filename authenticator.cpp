#include "Authenticator.h"

Authenticator::Authenticator()
{
   networkManager = new QNetworkAccessManager(this);
}

/*
 * login(username, password);
 * Attempts to access the login servers with authentication. Returns a QPair<bool, QString>
 * where bool is success, and QString is either the game access key or a specific error result
 * (e.g. invalid credentials, 2-factor required, other).
 *
 */
bool Authenticator::login(std::string username, std::string password) {

    if (username.compare("") == 0) { return false; }
    if (password.compare("") == 0) { return false; }

    // ttr api login
    QUrl apiUrl = QUrl("https://www.toontownrewritten.com/api/login?format=json");

    // build the post data
    QUrlQuery postData;
    postData.addQueryItem("username", QString::fromStdString(username));
    postData.addQueryItem("password", QString::fromStdString(password));

    // set up request
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());


    // post!
    networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(handleLoginResponse(QNetworkReply*)));

    return true;
}

/* handle the login response: there should be three responses
 * success = true - the user should be able to login (or delayed login)
 * success = false - something went wrong
 * success = partial - need authorization token
 */
void Authenticator::handleLoginResponse(QNetworkReply *reply) {
    qDebug() << "got a network response!";

    // if there is an error there's no need to continue
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonObject json_object = QJsonDocument::fromJson(data).object();

    // api documentation mentions that
    if (!json_object.contains("success")) {
        qDebug() << "key/value pair \"success\" is missing! stopping...";
        return;
    }

    QJsonValue res_success = json_object["success"];
    QJsonValue res_token;
    int status = -1;

    // user can immediately log in with given playcookie
    if (res_success.toString().compare("true") == 0) {
        res_token = json_object["cookie"];
        status = 0;
    }

    // user needs to go through 2-factor authentication
    if (res_success.toString().compare("partial") == 0) {
        res_token = json_object["responseToken"];
        status = 1;
    }

    // delayed response
    if (res_success.toString().compare("delayed") == 0) {
        res_token = json_object["queueToken"];
        status = 2;
    }

    // failure
    if (res_success.toString().compare("false") == 0) {
        res_token = json_object["success"];
        status = 3;
    }

    switch(status) {
    case 0:

    }
    reply->deleteLater();
}
