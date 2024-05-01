#ifndef ZEPTOGRAMSTATE_H
#define ZEPTOGRAMSTATE_H

#include <QObject>
#include "zeptogram/eventbus/event_bus.hpp"

using namespace dp;

class ZeptoGramState : public QObject
{
    Q_OBJECT
public:
    static ZeptoGramState* instance();

    event_bus* getEventBus()
    {
        return _eventBus;
    }
    
    bool isMainMenuOpened() const
    {
        return _mainMenuOpened;
    }

    void setMainMenuOpened(bool mainMenuOpened)
    {
        _mainMenuOpened = mainMenuOpened;
    }

    bool isSettingsOpened() const
    {
        return _settingsOpened;
    }

    void setSettingsOpened(bool settingsOpened)
    {
        _settingsOpened = settingsOpened;
    }

    bool isBotWebViewOpened() const
    {
        return _botWebViewOpened;
    }

    void setBotWebViewOpened(bool botWebViewOpened)
    {
        _botWebViewOpened = botWebViewOpened;
    }

    bool isChatSidePanelOpened() const
    {
        return _chatSidePanelOpened;
    }

    void setChatSidePanelOpened(bool chatSidePanelOpened)
    {
        _chatSidePanelOpened = chatSidePanelOpened;
    }

    bool isForceProxy() const
    {
        return _forceProxy;
    }

    void setForceProxy(bool forceProxy)
    {
        _forceProxy = forceProxy;
    }

    QString getProxy() const
    {
        return _proxy;
    }

    void setProxy(const QString& proxy)
    {
        _proxy = proxy;
    }

private:
    ZeptoGramState(QObject* parent = nullptr);

    inline static ZeptoGramState* inst = nullptr;
    bool _mainMenuOpened = false;
    bool _settingsOpened = false;
    bool _botWebViewOpened = false;
    bool _chatSidePanelOpened = false;
    bool _forceProxy = false;
    QString _proxy;

    event_bus* _eventBus = new event_bus();
};

#endif // ZEPTOGRAMSTATE_H
