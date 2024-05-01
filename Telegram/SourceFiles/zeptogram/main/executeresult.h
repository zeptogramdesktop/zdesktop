#ifndef EXECUTERESULT_H
#define EXECUTERESULT_H

#include <QJsonObject>

class ExecuteResult
{
public:
    ExecuteResult();

    void setType(const QString& type);
    QString getType();

    void setResult(const QJsonObject& result);
    QJsonObject getResult();

    void setErrorMessage(const QString& errorMessage);
    QString getErrorMessage();

    static ExecuteResult _false();
    static ExecuteResult _true();
    static ExecuteResult _false(const QString& error);
    static ExecuteResult _uint64(const uint64& val);

    static const QString _VOID;
    static const QString _BOOL;
    static const QString _STRING;
    static const QString _UINT64;

private:
    QString _type;
    QJsonObject _result;
    QString _errorMessage;
};

#endif // EXECUTERESULT_H
