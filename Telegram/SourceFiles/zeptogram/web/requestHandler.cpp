#include "requestHandler.h"

#include <QJsonArray>
#include "zeptogram/main/executeresult.h"
#include "zeptogram/magic_enum/magic_enum.hpp"


RequestHandler::RequestHandler()
{
    router.addRoute("GET", "/api/widgets", this, &RequestHandler::getWidgets);
    router.addRoute("GET", "/api/pages", this, &RequestHandler::getPages);
    router.addRoute("GET", "/api/page", this, &RequestHandler::getCurrentPage);
    router.addRoute("POST", "/api/command", this, &RequestHandler::executeCommand);
    router.addRoute("POST", "/api/scenario", this, &RequestHandler::executeScenario);
    router.addRoute("POST", "/api/selectchatbyusername", this, &RequestHandler::selectChatByUsername);
    router.addRoute("POST", "/api/selectchatbypeer", this, &RequestHandler::selectChatByPeerId);
    router.addRoute("POST", "/api/clickonmessage", this, &RequestHandler::clickOnMessage);
    router.addRoute("POST", "/api/clickoninlinebutton", this, &RequestHandler::clickOnInlineButton);
    router.addRoute("POST", "/api/getmessagescontaning", this, &RequestHandler::getMessagesContainingText);
    router.addRoute("GET", "/", this, &RequestHandler::getMainPage);
}

HttpPromise RequestHandler::handle(HttpDataPtr data)
{
    bool foundRoute;
    HttpPromise promise = router.route(data, &foundRoute);
    if (foundRoute)
        return promise;

    if (data->request->mimeType().compare("application/json", Qt::CaseInsensitive) != 0)
        throw HttpException(HttpStatus::BadRequest, "Request body content type must be application/json");

    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");

    return RequestHandler::ok(data);
}

HttpPromise RequestHandler::ok(HttpDataPtr data)
{
    return RequestHandler::ok(data, "OK");
}

HttpPromise RequestHandler::ok(HttpDataPtr data, QString result)
{
    QJsonObject object;
    object["status"] = 200;
    object["result"] = result;
    object["errorMessage"] = NULL;

    data->response->setStatus(HttpStatus::Ok, QJsonDocument(object));
    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::ok(HttpDataPtr data, bool result)
{
    QJsonObject object;
    object["status"] = 200;
    object["result"] = result;
    object["errorMessage"] = NULL;

    data->response->setStatus(HttpStatus::Ok, QJsonDocument(object));
    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::noOk(HttpDataPtr data, bool result, QString errorMessage)
{
    QJsonObject object;
    object["status"] = 400;
    object["result"] = result;
    object["errorMessage"] = errorMessage;

    data->response->setStatus(HttpStatus::Ok, QJsonDocument(object));
    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::ok(HttpDataPtr data, QJsonObject result)
{
    QJsonObject object;
    object["status"] = 200;
    object["result"] = result;
    object["errorMessage"] = NULL;

    data->response->setStatus(HttpStatus::Ok, QJsonDocument(object));
    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::proccessResult(ExecuteResult res, HttpDataPtr data)
{
    if (res.getType() == ExecuteResult::_VOID)
    {
        return RequestHandler::ok(data);
    }
    else if (res.getType() == ExecuteResult::_BOOL)
    {
        QJsonObject _result = res.getResult();
        QJsonValue _val = _result["result"];
        bool _st = _val.toBool();

        if (_st) {
            return RequestHandler::ok(data, true);
        }
        else
        {
            return RequestHandler::noOk(data, false, res.getErrorMessage());
        }
    }
    else if (res.getType() == ExecuteResult::_STRING)
    {
        return RequestHandler::ok(data, res.getResult());
    }
    else if (res.getType() == ExecuteResult::_UINT64)
    {
        return RequestHandler::ok(data, res.getResult());
    }

    return RequestHandler::ok(data);
}

HttpPromise RequestHandler::getCurrentPage(HttpDataPtr data)
{
    QString pageName = _executor->getCurrentPage();
   
    return RequestHandler::ok(data, pageName);
}

HttpPromise RequestHandler::getPages(HttpDataPtr data)
{
    auto pages = _executor->getPages();
    QJsonObject res;
    res["pages"] = QJsonArray::fromStringList(pages);

    return ok(data, res);
}

HttpPromise RequestHandler::getWidgets(HttpDataPtr data)
{
    auto widgets = _executor->getWidgets();
    QJsonObject res;
    res["widgets"] = QJsonArray::fromStringList(widgets);

    return ok(data, res);
}

HttpPromise RequestHandler::getMainPage(HttpDataPtr data)
{
    QJsonObject object;
    object["title"] = "ZeptoGram API";
    object["version"] = "0.0.1";

    return RequestHandler::ok(data, object);
}

HttpPromise RequestHandler::executeCommand(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue command = jsonDocument["command"];
    if (command == NULL || command.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Command must be not null");
    }

    if (!command.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "Command must be string");
    }

    QString strCommand = command.toString();
    QList<QString> commandArgs;

    QJsonValue args = jsonDocument["args"];
    if (args != NULL && !args.isNull())
    {
        if (args.isArray())
        {
            auto argsArray = args.toArray();
            for (int i = 0; i < argsArray.count(); i++)
            {
                commandArgs.append(argsArray.at(i).toString());
            }
        }
    }

    ExecuteResult res = _executor->executeCommand(strCommand, commandArgs);
    return RequestHandler::proccessResult(res, data);
}

HttpPromise RequestHandler::executeScenario(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue name = jsonDocument["name"];
    if (name == NULL || name.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Name must be not null");
    }

    if (!name.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "Name must be string");
    }

    std::string sName = name.toString().toStdString();
    auto eName = magic_enum::enum_cast<ExternalMethodType>(sName);
    if (!eName.has_value()) {
        throw HttpException(HttpStatus::BadRequest, "Unknown scenario");
    }

    _scService->executeScenario(eName.value());
    return RequestHandler::ok(data);
}

HttpPromise RequestHandler::selectChatByUsername(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue username = jsonDocument["username"];
    if (username == NULL || username.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Username must be not null");
    }

    if (!username.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "Username must be string");
    }

    ExecuteResult res = _executor->selectChatByUsername(username.toString());
    return RequestHandler::proccessResult(res, data);
}

HttpPromise RequestHandler::selectChatByPeerId(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue peerId = jsonDocument["peerId"];
    if (peerId == NULL || peerId.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "peerId must be not null");
    }

    if (!peerId.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "peerId must be string");
    }

    uint64 id = peerId.toString().toLongLong();
    ExecuteResult res = _executor->selectChatByPeerId(id);
    return RequestHandler::proccessResult(res, data);
}

HttpPromise RequestHandler::clickOnMessage(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue peerId = jsonDocument["peerId"];
    if (!peerId.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "peerId must be string");
    }

    QJsonValue messageText = jsonDocument["messageText"];
    if (!messageText.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "messageText must be string");
    }
    
    bool lb = true;
    QJsonValue leftButton = jsonDocument["leftButton"];
    if (!leftButton.isNull() && !leftButton.isUndefined())
    {
        if (!leftButton.isBool())
        {
            throw HttpException(HttpStatus::BadRequest, "leftButton must be boolean");
        }

        lb = leftButton.toBool();
    }

    double coef = 0.4;
    QJsonValue widthCoef = jsonDocument["widthCoef"];
    if (!widthCoef.isNull() && !widthCoef.isUndefined())
    {
        if (!widthCoef.isDouble())
        {
            throw HttpException(HttpStatus::BadRequest, "widthCoef must be double or int");
        }
    }
    coef = widthCoef.toDouble();

    uint64 id = peerId.toString().toLongLong();
    _executor->clickOnMessageWithText(id, messageText.toString(), lb, coef);
    return RequestHandler::ok(data);
}

HttpPromise RequestHandler::clickOnInlineButton(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue peerId = jsonDocument["peerId"];
    if (!peerId.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "peerId must be string");
    }

    QJsonValue messageText = jsonDocument["messageText"];
    if (!messageText.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "messageText must be string");
    }

    QJsonValue buttonText = jsonDocument["buttonText"];
    if (!buttonText.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "buttonText must be string");
    }

    bool lb = true;
    QJsonValue leftButton = jsonDocument["leftButton"];
    if (!leftButton.isNull() && !leftButton.isUndefined())
    {
        if (!leftButton.isBool())
        {
            throw HttpException(HttpStatus::BadRequest, "leftButton must be boolean");
        }

        lb = leftButton.toBool();
    }

    double coef = 0.4;
    QJsonValue widthCoef = jsonDocument["widthCoef"];
    if (!widthCoef.isNull() && !widthCoef.isUndefined())
    {
        if (!widthCoef.isDouble())
        {
            throw HttpException(HttpStatus::BadRequest, "widthCoef must be double or int");
        }
    }
    coef = widthCoef.toDouble();

    uint64 id = peerId.toString().toLongLong();
    _executor->clickOnInlineButtonWithText(id, messageText.toString(), buttonText.toString(), lb, coef);
    return RequestHandler::ok(data);
}

HttpPromise RequestHandler::getMessagesContainingText(HttpDataPtr data)
{
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull())
    {
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    QJsonValue peerId = jsonDocument["peerId"];
    if (!peerId.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "peerId must be string");
    }

    QJsonValue messageText = jsonDocument["messageText"];
    if (!messageText.isString())
    {
        throw HttpException(HttpStatus::BadRequest, "messageText must be string");
    }

    auto messagesList = _executor->getMessagesTextContaining(peerId.toString().toLongLong(), messageText.toString());

    QJsonObject object;
    object["status"] = 200;
    //object["result"] = result;
    object["errorMessage"] = NULL;

    QJsonArray jMessages;
    for (const auto& mes : messagesList) {
        QJsonObject jMes;

        int32 date = std::get<0>(mes);
        jMes.insert(QStringLiteral("date"), date);

        uint64 id = std::get<1>(mes);
        jMes.insert(QStringLiteral("id"), QString::number(id));

        QString messageText = std::get<2>(mes);
        jMes.insert(QStringLiteral("text"), messageText);
        jMessages.append(jMes);
    }
    object["result"] = jMessages;

    data->response->setStatus(HttpStatus::Ok, QJsonDocument(object));
    return HttpPromise::resolve(data);
}
