#ifndef FILEUPDATER_H
#define FILEUPDATER_H

#include <QObject>
#include <QtNetwork>

class FileUpdater : public QObject
{
    Q_OBJECT

public:
    FileUpdater();
    ~FileUpdater();

    void download_files(QStringList filenames);

private slots:
    void handle_network_response(QNetworkReply *reply);
private:
    QNetworkAccessManager *networkManager;
    void download_file(std::string filename);
};

#endif // FILEUPDATER_H
