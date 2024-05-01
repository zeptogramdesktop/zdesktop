#ifndef HTTP_SERVER_HTTPSERVER_H
#define HTTP_SERVER_HTTPSERVER_H

#include "httpConnection.h"
#include "httpServerConfig.h"
#include "httpRequestHandler.h"
#include "util.h"

#include <QBasicTimer>
#include <QtNetwork/qtcpserver.h>


// HTTP server is HTTP/1.1 compliant and is based on RFC7230 series. This specification was created in June 2014 and
// obsoletes RFC2616, RFC2145
class HttpServer : public QTcpServer
{
    Q_OBJECT

private:
    HttpServerConfig config;
    HttpRequestHandler *requestHandler;

    std::vector<HttpConnection *> connections;

public:
    HttpServer(const HttpServerConfig &config, HttpRequestHandler *requestHandler, QObject *parent = nullptr);
    ~HttpServer();

    bool listen();
    void close();

protected:
    void incomingConnection(qintptr socketDescriptor);

private Q_SLOTS:
    void connectionDisconnected();

Q_SIGNALS:
    void handleConnection(int socketDescriptor);

};

#endif // HTTP_SERVER_HTTPSERVER_H
