#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QtPromise>
#include <QTimer>

#include "zeptogram/httpServer/httpData.h"
#include "zeptogram/httpServer/httpRequestHandler.h"
#include "zeptogram/httpServer/httpRequestRouter.h"
#include "zeptogram/zeptogramexecutor.h"
#include "zeptogram/main/scenarioservice.h"


using QtPromise::QPromise;

class RequestHandler : public HttpRequestHandler
{
private:
    HttpRequestRouter router;
    ZeptoGramExecutor* _executor = ZeptoGramExecutor::instance();
    ScenarioService* _scService = ScenarioService::instance();


    static HttpPromise ok(HttpDataPtr data);
    static HttpPromise ok(HttpDataPtr data, QString result);
    static HttpPromise ok(HttpDataPtr data, bool result);
    static HttpPromise noOk(HttpDataPtr data, bool result, QString errorMessage);
    static HttpPromise ok(HttpDataPtr data, QJsonObject result);
    static HttpPromise proccessResult(ExecuteResult res, HttpDataPtr data);

public:
    RequestHandler();
    HttpPromise handle(HttpDataPtr data);

    HttpPromise getMainPage(HttpDataPtr data);
    HttpPromise getCurrentPage(HttpDataPtr data);
    HttpPromise getPages(HttpDataPtr data);
    HttpPromise getWidgets(HttpDataPtr data);

    HttpPromise executeCommand(HttpDataPtr data);
    HttpPromise executeScenario(HttpDataPtr data);
    HttpPromise selectChatByUsername(HttpDataPtr data);
    HttpPromise selectChatByPeerId(HttpDataPtr data);
    HttpPromise clickOnMessage(HttpDataPtr data);
    HttpPromise clickOnInlineButton(HttpDataPtr data);
    HttpPromise getMessagesContainingText(HttpDataPtr data);
};

#endif // REQUESTHANDLER_H
