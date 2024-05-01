#include "zeptogramserver.h"

#include <QFile>
#include <QDebug>
#include <QSettings>

ZeptoGramServer::ZeptoGramServer(QObject *parent) : QObject(parent)
{
}

void ZeptoGramServer::start(QApplication* app)
{
	qDebug() << "ZPT: Starting HTTP Web Server";

    /*HttpServerConfig config;
    config.port = 8080;
    config.requestTimeout = 20;
    config.responseTimeout = 5;
    config.verbosity = HttpServerConfig::Verbose::All;
    config.maxMultipartSize = 512 * 1024 * 1024;*/

    if (app == NULL)
    {
        qDebug() << "Cannot initialize HttpServer, exiting";
        return;
    }

    HttpServerConfig config = loadSettingsForHttpServerConfig(app);
    QString binPath = app->applicationDirPath();
    if (binPath != NULL && !binPath.isEmpty())
    {
        config.errorDocumentMap[HttpStatus::NotFound] = binPath + "/" + "data/404_2.html";
        config.errorDocumentMap[HttpStatus::InternalServerError] = binPath + "/" + "/data/404_2.html";
        config.errorDocumentMap[HttpStatus::BadGateway] = binPath + "/" + "/data/404_2.html";
    }

    _requestHandler = new RequestHandler();
    HttpServer* server = new HttpServer(config, _requestHandler);
    server->listen();
}

HttpServerConfig ZeptoGramServer::loadSettingsForHttpServerConfig(QApplication* app)
{
    QString binDir = app->applicationDirPath();
    QString filePath = binDir + "/zeptogram.ini";
    QFile configFile(filePath);

    if (configFile.exists())
    {
        // load settings from file
        return loadConfigFromFile(filePath);
    }
    else
    {
        // create default config file
        // and load settings from it
        createDefaultConfigFile(filePath);
        return loadConfigFromFile(filePath);
    }
}

void ZeptoGramServer::createDefaultConfigFile(const QString& file)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup("HttpServer");
    settings.setValue("host", "localhost");
    settings.setValue("port", 8080);
    settings.setValue("requestTimeout", 20);
    settings.setValue("responseTimeout", 5);
    settings.setValue("verbosity", verbosityToString(HttpServerConfig::Verbose::All));
    settings.setValue("maxMultipartSize", 512 * 1024 * 1024);
    settings.endGroup();
}

HttpServerConfig ZeptoGramServer::loadConfigFromFile(const QString& file)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup("HttpServer");

    HttpServerConfig config;
    config.port = settings.value("port").toInt();
    config.host = QHostAddress(settings.value("host").toString());
    config.requestTimeout = settings.value("requestTimeout").toInt();
    config.responseTimeout = settings.value("responseTimeout").toInt();
    config.verbosity = stringToVerbosity(settings.value("verbosity").toString());
    config.maxMultipartSize = settings.value("maxMultipartSize").toInt();
    settings.endGroup();

    return config;
}

const QString ZeptoGramServer::verbosityToString(HttpServerConfig::Verbose v)
{
    switch (v)
    {
    case HttpServerConfig::Verbose::None:
        return "None";
    case HttpServerConfig::Verbose::Critical:
        return "Critical";
    case HttpServerConfig::Verbose::Warning:
        return "Warning";
    case HttpServerConfig::Verbose::Info:
        return "Info";
    case HttpServerConfig::Verbose::Debug:
        return "Debug";
    case HttpServerConfig::Verbose::All:
        return "All";
    default:
        return "All";
    }
}

const HttpServerConfig::Verbose ZeptoGramServer::stringToVerbosity(const QString & level)
{
    if (level.isEmpty() || level.isNull())
    {
        return HttpServerConfig::Verbose::All;
    }

    if (level == "None")
    {
        return HttpServerConfig::Verbose::None;
    }
    else if (level == "Critical")
    {
        return HttpServerConfig::Verbose::Critical;
    }
    else if (level == "Warning")
    {
        return HttpServerConfig::Verbose::Warning;
    }
    else if (level == "Info")
    {
        return HttpServerConfig::Verbose::Info;
    }
    else if (level == "Debug")
    {
        return HttpServerConfig::Verbose::Debug;
    }
    else if (level == "All")
    {
        return HttpServerConfig::Verbose::All;
    }
    
    return HttpServerConfig::Verbose::All;
}

RequestHandler* ZeptoGramServer::getRequestHandler()
{
    return _requestHandler;
}
