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
    void download_files_request(std::vector<std::pair<std::string, std::string>> filenames_pair);
    void update_download_request(double progress, std::string text);
private slots:
    void handle_network_response(QNetworkReply *reply);
private:
    QNetworkAccessManager *networkManager;
    bool file_up_to_date(std::string path, std::string dl_hash);
};

#endif // WEBUPDATER_H
