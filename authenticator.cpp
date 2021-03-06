#include "Authenticator.h"
#include <stdlib.h>
#include <QProcess>

Authenticator::Authenticator() {
   networkManager = new QNetworkAccessManager(this);
   connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(handle_auth_response(QNetworkReply*)));
}

Authenticator::~Authenticator() {
    delete networkManager;
}

/*
 * login(username, password);
 * Attempts to access the login servers with authentication. Returns a QPair<bool, QString>
 * where bool is success, and QString is either the game access key or a specific error result
 * (e.g. invalid credentials, 2-factor required, other).
 *
 */
bool Authenticator::login(std::string username, std::string password) {

    if (username.length() == 0 || password.length() == 0) { return false; }

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
    return true;
}

/* handle the network response: there should be four responses
 * success = true - the user got a cookie and can now play the game
 * success = false - something went wrong (wrong username/password or something else)
 * success = partial - need authorization token to get a cookie
 * success = delayed - need to wait in queue to get a cookie
 */
void Authenticator::handle_auth_response(QNetworkReply *reply) {
    // if there is an error there's no need to continue
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonObject json_object = QJsonDocument::fromJson(data).object();
    reply->deleteLater();

    qDebug() << QString(data);
    // api documentation mentions that there should always be a success key/value pair.
    if (!json_object.contains("success")) {
        qDebug() << "key/value pair \"success\" is missing. stopping...";
        return;
    }

    QJsonValue res_success = json_object["success"];

    // user can immediately log in with given playcookie
    if (res_success.toString() == "true") {
        qDebug() << "player cookie get!";
        launch_game(json_object["cookie"].toString().toStdString(), json_object["gameserver"].toString().toStdString());
        return;
    }

    // user needs to go through 2-factor authentication
    if (res_success.toString() == "partial") {
        qDebug() << "awaiting two factor...";
        two_factor_server_token = json_object["responseToken"].toString().toStdString();
        two_factor();
    }

    // delayed response
    if (res_success.toString() == "delayed") {
        qDebug() << "in queue...";
        delayed_login(json_object["queueToken"].toString().toStdString());
    }

    // failure
    if (res_success.toString() == "false") {
        emit login_status_update_request(json_object["banner"].toString().toStdString());
        emit reset_login_request();
        qDebug() << "Unable to login!";
        qDebug() << json_object["banner"].toString();
        return;
    }
}

/* two_factor(server_token, player_token);
 * posts to server the appToken (given by server previously) and the authToken (given by user)
 */
void Authenticator::two_factor(std::string player_token) {
    if (player_token.compare("") == 0) {
        qDebug() << "need player token to proceed, opening popup...";
        emit login_status_update_request("Awaiting two-factor....");
        emit two_factor_request();
        return;
    }

    // ttr api
    QUrl apiUrl = QUrl("https://www.toontownrewritten.com/api/login?format=json");

    // build the post data
    QUrlQuery postData;
    postData.addQueryItem("appToken", QString::fromStdString(player_token));
    postData.addQueryItem("authToken", QString::fromStdString(two_factor_server_token));

    // set up request
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    // post!
    networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
}

/*
 * delayed_login(queue_token)
 * sends a request every x seconds given the queue token
 * to let the player in as soon as possible without making
 * the server angry at us for requesting so many times
 */
void Authenticator::delayed_login(std::string queue_token) {

    emit login_status_update_request("in queue...");
    qDebug() << "Received delayed login request, sleeping for 3 seconds before sending another...";
    this->thread()->sleep(3);

    // ttr api
    QUrl apiUrl = QUrl("https://www.toontownrewritten.com/api/login?format=json");

    // build the post data
    QUrlQuery postData;
    postData.addQueryItem("queueToken", QString::fromStdString(queue_token));

    // set up request
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    // post!
    networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
}

/* launch the game with environments set */
void Authenticator::launch_game(std::string player_cookie, std::string gameserver_ip) {
    QProcess qp;
    qputenv("TTR_GAMESERVER", QString::fromStdString(gameserver_ip).toUtf8());
    qputenv("TTR_PLAYCOOKIE", QString::fromStdString(player_cookie).toUtf8());

    qDebug() << "launching...";

    emit login_status_update_request("Authentication successful. See you in game!");
    // let's safely assume the launcher is in the same directory as the game
    qp.startDetached("TTREngine.exe", QStringList());

    emit reset_login_request();
    emit login_success();
}
