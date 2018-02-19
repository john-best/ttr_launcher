#ifndef FILEUPDATER_H
#define FILEUPDATER_H

#include <QObject>
#include <QtNetwork>
#include <unordered_map>

class FileUpdater : public QObject
{
    Q_OBJECT

public:
    FileUpdater();
    ~FileUpdater();

    void download_files(std::vector<std::pair<std::string, std::string>> dl_filenames);

private slots:
    void handle_network_response(QNetworkReply *reply);
private:
    QNetworkAccessManager *networkManager;
    void download_file(std::string filename);
    void extract(std:: string filename);
    std::unordered_map<std::string, std::string> bz2_to_files;
};

#endif // FILEUPDATER_H
