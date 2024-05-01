#ifndef ZEPTOGRAMEXECUTOR_H
#define ZEPTOGRAMEXECUTOR_H

#include <QString>
#include <QObject>

#include "zeptogram/constants/widgettypes.h"
#include "zeptogram/main/executeresult.h"
#include "zeptogram/constants/eventtypes.h"

using namespace zeptogram;

/*
* Singleton command executor
*/

struct UiDelegate {
    QString pageName;
    QWidget* widget;

    UiDelegate(QString pageName, QWidget* widget)
    {
        this->pageName = pageName;
        this->widget = widget;
    }
};

struct Executable {
    QWidget* _widget;
    ExternalMethodType _method;
    
    Executable(QWidget* widget, ExternalMethodType method)
    {
        _widget = widget;
        _method = method;
    }
};

struct WidgetAndType {
    bool _empty = true;
    QWidget* _widget;
    WIDGET_TYPE _widgetType;

    WidgetAndType()
    {
        _empty = true;
        _widget = NULL;
        _widgetType = WIDGET_TYPE::BUTTON;
    }

    WidgetAndType(QWidget* widget, WIDGET_TYPE widgetType)
    {
        _widget = widget;
        _widgetType = widgetType;
        this->_empty = false;
    }

    bool isEmpty() const
    {
        return _empty;
    }
};

struct ArgsValidation {
    int result;
    QString errorMessage;

    ArgsValidation(int _result)
    {
        result = _result;
    }

    ArgsValidation(int _result, QString _errorMessage)
    {
        result = _result;
        errorMessage = _errorMessage;
    }
};

class ZeptoGramExecutor : public QObject
{
    Q_OBJECT

public:
    static ZeptoGramExecutor* instance();
    void registerPageable(UiDelegate uiDelegate);
    void registerWidget(QWidget* widget, const QString& widgetName, WIDGET_TYPE wType);
    ExecuteResult executeCommand(const QString& command, QList<QString> args);
    ExecuteResult selectChatByUsername(const QString& username);
    ExecuteResult selectChatByPeerId(const uint64& peerId);
    void clickOnMessageWithText(uint64 peerId, const QString& messageText, const bool leftButton = true, 
        const double widthDiffCoef = 0.4);
    void clickOnInlineButtonWithText(int64 peerId, const QString& messageText, const QString& btnText,
        const bool leftButton = true, const double widthDiffCoef = 0.5);
    std::vector<std::tuple<int32, uint64, QString>> getMessagesTextContaining(uint64 peerId, const QString& messageText);

    QString getCurrentPage() const;
    QStringList getPages() const;
    QStringList getWidgets() const;

public Q_SLOTS:
    void onPageChanged();

private:
    explicit ZeptoGramExecutor();
    inline static ZeptoGramExecutor* inst;

    WidgetAndType findWidgetByName(QString name);
    int getMethodNameByName(QString name);
    ExecuteResult executeMethodForWidget(QWidget* widget, QString widgetId, WIDGET_TYPE wType, QString method, QList<QString> args);
    ExecuteResult doExecuteMethodForWidget(QWidget* widget, WIDGET_TYPE wType, METHOD_NAME methodName, QList<QString> args);

    ArgsValidation validateArgsForWidget(WIDGET_TYPE wType, QList<QString> args);

    QMap<QString, QWidget*> widgetNames;
    QMap<QString, WidgetAndType> registeredWidgets;
    QList<UiDelegate> pageables;
    QString currentPageName;

Q_SIGNALS:
    void changed(QString page);
};

#endif // ZEPTOGRAMEXECUTOR_H
