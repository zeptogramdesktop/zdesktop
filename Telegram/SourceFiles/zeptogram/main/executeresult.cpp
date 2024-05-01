#include "executeresult.h"

const QString ExecuteResult::_VOID = "VOID";
const QString ExecuteResult::_BOOL = "BOOL";
const QString ExecuteResult::_STRING = "STRING";
const QString ExecuteResult::_UINT64 = "UINT64";

ExecuteResult::ExecuteResult()
{
}

ExecuteResult ExecuteResult::_false()
{
	QJsonObject obj;
	obj["result"] = false;

	ExecuteResult res;
	res.setType(ExecuteResult::_BOOL);
	res.setResult(obj);
	return res;
}

ExecuteResult ExecuteResult::_true()
{
	QJsonObject obj;
	obj["result"] = true;

	ExecuteResult res;
	res.setType(ExecuteResult::_BOOL);
	res.setResult(obj);
	return res;
}

ExecuteResult ExecuteResult::_false(const QString& error)
{
	QJsonObject obj;
	obj["result"] = false;

	ExecuteResult res;
	res.setType(ExecuteResult::_BOOL);
	res.setResult(obj);
	res.setErrorMessage(error);
	return res;
}

ExecuteResult ExecuteResult::_uint64(const uint64& val)
{
	QJsonObject obj;
	obj["result"] = QString::number(val);

	ExecuteResult res;
	res.setType(ExecuteResult::_UINT64);
	res.setResult(obj);
	return res;
}

void ExecuteResult::setType(const QString& type)
{
	_type = type;
}

void ExecuteResult::setResult(const QJsonObject& result)
{
	_result = result;
}

QString ExecuteResult::getType()
{
	return _type;
}

QJsonObject ExecuteResult::getResult()
{
	return _result;
}

void ExecuteResult::setErrorMessage(const QString& errorMessage)
{
	_errorMessage = errorMessage;
}

QString ExecuteResult::getErrorMessage()
{
	return _errorMessage;
}
