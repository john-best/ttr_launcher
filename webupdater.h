#ifndef WEBUPDATER_H
#define WEBUPDATER_H

#include <QtNetwork>
#include <string.h>

class WebUpdaterWorker : public QObject {
    Q_OBJECT
    QThread workerThread;

public slots:
    void handle_manifest_worker(QByteArray data);
signals:
    void resultReady(std::vector<std::pair<std::string, std::string>> needs_dl);
    void update_download_request_worker(double progress, std::string text);
private:
    bool file_up_to_date(std::string path, std::string dl_hash);
};

class WebUpdater : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    WebUpdater();
    ~WebUpdater();
    void get_news();
    void update_manifest();

signals:
    void update_news_request(bool res, std::string in);
    void download_files_request(std::vector<std::pair<std::string, std::string>> filenames_pair);
    void update_download_request(double progress, std::string text);
    void handle_manifest(QByteArray data);
private slots:
    void handle_network_response(QNetworkReply *reply);
    void handle_download_request(double progress, std::string text);
    void handle_finished_manifest_request(std::vector<std::pair<std::string, std::string>> filenames_pair);
private:
    QNetworkAccessManager *networkManager;
};

#endif // WEBUPDATER_H
