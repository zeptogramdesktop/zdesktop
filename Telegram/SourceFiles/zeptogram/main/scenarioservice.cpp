#define zDebug() qDebug() << Q_FUNC_INFO

#include "scenarioservice.h"

#include <QDebug>

using namespace zeptogram;

ScenarioService::ScenarioService(QObject *parent) : QObject{parent}
{
}

void ScenarioService::executeScenario(ExternalMethodType type)
{
	if (type == ExternalMethodType::OPEN_MAIN_MENU)
	{
		openMainMenu();
		return;
	}
	else if (type == ExternalMethodType::CLOSE_MAIN_MENU)
	{
		closeMainMenu();
		return;
	}
	else if (type == ExternalMethodType::OPEN_SETTINGS)
	{
		openSettings();
	}
}

void ScenarioService::openMainMenu()
{
	QList<QString> args;
	_exec->executeCommand("mainMenuButton.click()", args);
}

void ScenarioService::closeMainMenu()
{
	QList<QString> args;
	_exec->executeCommand("mainLayerWidget.click()", args);
}

void ScenarioService::openSettings()
{
	bool isOpened = _state->isMainMenuOpened();
	if (isOpened) {
		QList<QString> args;
		_exec->executeCommand("mainMenuSettingsButton.click()", args);
	}
	else
	{
		openMainMenu();
		_waitForMainMenuOpenedForSettings = true;
	}
}

// slot
void ScenarioService::mainMenuOpened()
{
	zDebug() << "ZPT: Received signal mainMenuOpened";

	if (_waitForMainMenuOpenedForSettings)
	{
		qDebug() << "ZPT: Do nothing";
		return;
	}

	qDebug() << "ZPT: Main menu opened";
}

ScenarioService* ScenarioService::instance()
{
	if (inst == nullptr)
	{
		inst = new ScenarioService(0);
	}
	
	return inst;
}