#ifndef WEBUPDATER_H
#define WEBUPDATER_H

#include <QtNetwork>
#include <string.h>

class WebUpdaterWorker : public QObject {
    Q_OBJECT
    QThread workerThread;

public slots:
    void worker_handle_manifest(QByteArray data);
signals:
    void worker_required_files_request(std::vector<std::pair<std::string, std::string>> needs_dl);
    void worker_update_download_request(double progress, std::string text);
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
    void update_manifest();

signals:
    void download_files_request(std::vector<std::pair<std::string, std::string>> filenames_pair);
    void update_download_request(double progress, std::string text);
    void send_manifest_to_worker(QByteArray data);
private slots:
    void handle_network_response(QNetworkReply *reply);
    void send_update_download_request_to_main(double progress, std::string text);
    void send_required_files_to_main(std::vector<std::pair<std::string, std::string>> filenames_pair);
private:
    QNetworkAccessManager *networkManager;
};

#endif // WEBUPDATER_H
