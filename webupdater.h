#ifndef WEBUPDATER_H
#define WEBUPDATER_H

#include <QtNetwork>

class WebUpdater : public QObject
{
    Q_OBJECT

public:
    WebUpdater();
    ~WebUpdater();
    bool load_news();
};

#endif // WEBUPDATER_H
