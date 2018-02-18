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
};

#endif // FILEUPDATER_H
