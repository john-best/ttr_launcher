#ifndef FILEUPDATER_H
#define FILEUPDATER_H

#include <QObject>
#include <QtNetwork>
#include <unordered_map>
class FileUpdaterWorker : public QObject {
    Q_OBJECT
    QThread workerThread;

signals:
    void worker_update_download_request(double progress, std::string text);
public slots:
    void worker_handle_file(QByteArray data, std::string filename, std::unordered_map<std::string, std::string> bz2_to_files);
    void extract(std:: string filename, std::unordered_map<std::string, std::string> bz2_to_files);
private:
    size_t file_completed_count = 0;
};


class FileUpdater : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    FileUpdater();
    ~FileUpdater();

    void download_files(std::vector<std::pair<std::string, std::string>> dl_filenames);

signals:
    void update_download_request(double progress, std::string text);
    void send_file_to_worker(QByteArray data, std::string filename, std::unordered_map<std::string, std::string> bz2_to_files);
private slots:
    void handle_network_response(QNetworkReply *reply);
    void send_update_download_request_to_main(double progress, std::string text);
private:
    QNetworkAccessManager *networkManager;
    void download_file(std::string filename);
    std::unordered_map<std::string, std::string> bz2_to_files;
};

#endif // FILEUPDATER_H
