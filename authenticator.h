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
    bool login(std::string username, std::string password);

private:
    QNetworkAccessManager *networkManager;

private slots:
    void handleLoginResponse(QNetworkReply *reply);
};

#endif // AUTHENTICATOR_H
