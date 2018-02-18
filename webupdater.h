#ifndef WEBUPDATER_H
#define WEBUPDATER_H

#include <QtNetwork>
#include <string.h>
class WebUpdater : public QObject
{
    Q_OBJECT

public:
    WebUpdater();
    ~WebUpdater();
    void get_news();
    void update_manifest();

signals:
    void update_news_request(bool res, std::string in);
private slots:
    void handle_network_response(QNetworkReply *reply);
private:
    QNetworkAccessManager *networkManager;
    bool file_up_to_date(std::string path, std::string dl_hash);
    std::string get_bz2_hash(std::string path);
};

#endif // WEBUPDATER_H
