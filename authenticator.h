#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QtNetwork>
#include <string>
#include <utility>

class Authenticator : public QObject
{
    Q_OBJECT
public:
    Authenticator();
    ~Authenticator();
    bool login(std::string username, std::string password);
    void two_factor(std::string player_token = "");
    void delayed_login(std::string queue_token);
    void launch_game(std::string player_cookie);

signals:
    void two_factor_request();

private:
    QNetworkAccessManager *networkManager;
    std::string two_factor_server_token = "";
private slots:
    void handle_auth_response(QNetworkReply *reply);
};

#endif // AUTHENTICATOR_H