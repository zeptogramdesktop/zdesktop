#include "httpServer.h"

HttpServer::HttpServer(const HttpServerConfig &config, HttpRequestHandler *requestHandler, QObject *parent) :
    QTcpServer(parent), config(config), requestHandler(requestHandler)
{
    setMaxPendingConnections(config.maxPendingConnections);
}

bool HttpServer::listen()
{
    if (!QTcpServer::listen(config.host, config.port))
    {
        if (config.verbosity >= HttpServerConfig::Verbose::Warning)
        {
            qWarning().noquote() << QString("Unable to listen on %1:%2: %3").arg(config.host.toString())
                .arg(config.port).arg(errorString());
        }

        return false;
    }

    if (config.verbosity >= HttpServerConfig::Verbose::Info)
        qInfo() << "Listening...";

    return true;
}

void HttpServer::close()
{
    QTcpServer::close();
}

void HttpServer::incomingConnection(qintptr socketDescriptor)
{
    if ((int)connections.size() >= config.maxConnections)
    {
        // Create TCP socket
        // Delete the socket automatically once a disconnected signal is received
        QTcpSocket *socket = new QTcpSocket(this);
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

        if (!socket->setSocketDescriptor(socketDescriptor))
        {
            if (config.verbosity >= HttpServerConfig::Verbose::Critical)
                qCritical() << QString("Invalid socket descriptor given (%1)").arg(socket->errorString());

            return;
        }

        if (config.verbosity >= HttpServerConfig::Verbose::Warning)
        {
            qWarning() << QString("Maximum connections reached (%1). Rejecting connection from %2")
                .arg(config.maxConnections).arg(socket->peerAddress().toString());
        }

        HttpResponse *response = new HttpResponse(&config);
        response->setError(HttpStatus::ServiceUnavailable, "Too many connections", true);
        response->prepareToSend();

        // Assume that the entire request will be written in one go, relatively safe assumption
        response->writeChunk(socket);
        delete response;

        // This will disconnect after all bytes have been written
        socket->disconnectFromHost();
        return;
    }

    HttpConnection *connection = new HttpConnection(&config, requestHandler, socketDescriptor);
    connect(connection, &HttpConnection::disconnected, this, &HttpServer::connectionDisconnected);
    connections.push_back(connection);
}

void HttpServer::connectionDisconnected()
{
    HttpConnection *connection = dynamic_cast<HttpConnection *>(sender());
    if (!connection)
        return;

    // Remove connection from connections list
    auto it = std::find(connections.begin(), connections.end(), connection);
    if (it != connections.end())
        connections.erase(it);

    // We do delete later here because if this signal was emitted while socket is disconnecting, it still needs the
    // socket reference for a bit
    connection->deleteLater();
}

HttpServer::~HttpServer()
{
    for (HttpConnection *connection : connections)
        delete connection;

    close();
}
