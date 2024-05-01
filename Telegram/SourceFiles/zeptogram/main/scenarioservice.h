#ifndef SCENARIOSERVICE_H
#define SCENARIOSERVICE_H

#include <QObject>

#include "zeptogram/constants/eventtypes.h"
#include "zeptogram/zeptogramexecutor.h"
#include "zeptogram/state/zeptogramstate.h"

class ScenarioService : public QObject
{
    Q_OBJECT
public:
    static ScenarioService* instance();
    void executeScenario(ExternalMethodType type);
    void openMainMenu();
    void closeMainMenu();
    void openSettings();

public Q_SLOTS:
    void mainMenuOpened();

private:
    ScenarioService(QObject* parent = nullptr);
    inline static ScenarioService* inst = nullptr;

    ZeptoGramExecutor* _exec = ZeptoGramExecutor::instance();
    ZeptoGramState* _state = ZeptoGramState::instance();

    bool _waitForMainMenuOpenedForSettings = false;
};

#endif // SCENARIOSERVICE_H
