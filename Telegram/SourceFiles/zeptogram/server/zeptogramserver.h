#ifndef ZEPTOGRAMSERVER_H
#define ZEPTOGRAMSERVER_H

#include <QApplication>
#include "zeptogram/web/requestHandler.h"
#include "zeptogram/httpServer/httpServer.h"

class ZeptoGramServer : public QObject
{
    Q_OBJECT
public:
    explicit ZeptoGramServer(QObject *parent = 0);
    void start(QApplication *app);
    RequestHandler* getRequestHandler();

private:
    RequestHandler* _requestHandler;
    HttpServerConfig loadSettingsForHttpServerConfig(QApplication* app);
    void createDefaultConfigFile(const QString& file);
    HttpServerConfig loadConfigFromFile(const QString& file);

    const QString verbosityToString(HttpServerConfig::Verbose v);
    const HttpServerConfig::Verbose stringToVerbosity(const QString& level);
};

#endif // ZEPTOGRAMSERVER_H
