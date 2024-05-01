#include "zeptogramexecutor.h"

#include <QList>
#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>

#include "ui/widgets/labels.h"
#include "ui/abstract_button.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/multi_select.h"
#include "ui/countryinput.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/fields/special_fields.h"
#include "ui/widgets/scroll_area.h"
#include "ui/layers/layer_widget.h"
#include "ui/rp_widget.h"
#include "intro/intro_code_input.h"
#include "dialogs/dialogs_widget.h"
#include "dialogs/dialogs_inner_widget.h"
#include "data/data_peer.h"
#include "dialogs/dialogs_row.h"
#include "ui/widgets/checkbox.h"
#include "mtproto/mtproto_proxy_data.h"
#include "settings/settings_information.h"
#include "history/history_inner_widget.h"

#include "zeptogram/main/pageable.h"
#include "zeptogram/main/pagechangeable.h"
#include "zeptogram/constants/pageconstants.h"
#include "zeptogram/main/widgetexecutor.h"
#include "zeptogram/constants/eventtypes.h"
#include "zeptogram/main/externalcommands.h"
#include "zeptogram/state/zeptogramstate.h"

#include "zeptogram/magic_enum/magic_enum.hpp"

#include <string>
#include <QPoint>

using namespace zeptogram;
using ProxyData = MTP::ProxyData;

ZeptoGramExecutor::ZeptoGramExecutor()
{
}

ZeptoGramExecutor* ZeptoGramExecutor::instance()
{
    if (inst != NULL)
    {
        return inst;
    }

    inst = new ZeptoGramExecutor();
    return inst;
}

void ZeptoGramExecutor::registerPageable(UiDelegate uiDelegate)
{
    qDebug() << "ZPT: Registering delegate widget: " << uiDelegate.pageName;
    QWidget* widget = uiDelegate.widget;
    Pageable* pageable = dynamic_cast<Pageable*>(widget);
    if (pageable) 
    {
        PageChangeable* emitter = pageable->getEmitter();
        if (emitter != NULL)
        {
            connect(emitter, &PageChangeable::pageChanged, inst, &ZeptoGramExecutor::onPageChanged);
        }
    }

    pageables.append(uiDelegate);
}

void ZeptoGramExecutor::registerWidget(QWidget* widget, const QString& widgetName, WIDGET_TYPE wType)
{
    if (widget == NULL)
    {
        qDebug() << "Widget to register cannot be null!";
        return;
    }

    if (widgetName == NULL || widgetName.isEmpty() || widgetName.isNull())
    {
        qDebug() << "ZPT: Unable to register widget: Widget name cannot be null!";
        return;
    }

    widget->setObjectName(widgetName);
    registeredWidgets[widgetName] = WidgetAndType(widget, wType);
}

void ZeptoGramExecutor::onPageChanged()
{
    qDebug() << "Page changed";
    QObject* obj = sender();
    if (obj == NULL)
    {
        qDebug() << "Sender is NULL, cannot proceed";
        return;
    }

    PageChangeable* emitter = qobject_cast<PageChangeable*>(obj);
    if (emitter == NULL)
    {
        qDebug() << "Emitter is NULL, cannot proceed";
        return;
    }

    Pageable* pageable = dynamic_cast<Pageable*>(emitter->parent());
    if (pageable == NULL)
    {
        qDebug() << "Pageable is NULL, cannot proceed";
        return;
    }

    QString pageName = pageable->getPage();
    qDebug() << "Setting current page for: " << pageName;
    this->currentPageName = pageName;
}

QString ZeptoGramExecutor::getCurrentPage() const
{
    qDebug() << "ZPT: Returning currentPage " << this->currentPageName;
    return this->currentPageName;
}

QStringList ZeptoGramExecutor::getPages() const
{
    qDebug() << "ZPT: Returning all pages names";
    QStringList pages;

    for (auto const& page : PAGES) {
        pages.append(QString::fromStdString(page));
    }
    return pages;
}

QStringList ZeptoGramExecutor::getWidgets() const
{
    qDebug() << "ZPT: Returning all widgets names";
    QStringList widgets;

    for (auto const& w : WIDGETS) {
        widgets.append(QString::fromStdString(w));
    }
    return widgets;
}

int ZeptoGramExecutor::getMethodNameByName(QString name)
{
    auto& reflector = EnumReflector::For<::zeptogram::METHOD_NAME>();
    for (auto& val : reflector)
    {
        QString enumName = QString::fromStdString(val.Name());
        enumName = enumName.replace("_", "").simplified().toLower();
        if (name.simplified().toLower().contains(enumName))
        {
            return val.Value();
        }
    }

    return -1;
}

ExecuteResult ZeptoGramExecutor::executeMethodForWidget(QWidget* widget, QString widgetId, WIDGET_TYPE wType, QString method, QList<QString> args)
{
    if (!AVAILABLE_METHODS.contains(QString::number(wType)))
    {
        qDebug() << "ZPT: Not found any available methods for WIDGET_TYPE " << widgetId;
        return ExecuteResult::_false("Not found any available for WIDGET_TYPE " + widgetId);
    }

    QList<METHOD_NAME> methods = AVAILABLE_METHODS.value(QString::number(wType));
    int mNumber = getMethodNameByName(method);
    if (mNumber == -1)
    {
        qDebug() << "ZPT: Unknown METHOD_NAME: " << method;
        return ExecuteResult::_false("Unknown METHOD_NAME : " + method);
    }

    METHOD_NAME realMethod;
    bool mFound = false;
    for (METHOD_NAME mn : methods)
    {
        if (mn == mNumber)
        {
            mFound = true;
            realMethod = mn;
            break;
        }
    }

    if (!mFound)
    {
        qDebug() << "ZPT: Method " << method << " is not available for WIDGET_TYPE " << widgetId;
        return ExecuteResult::_false("Method " + method + " is not available for WIDGET_TYPE " + widgetId);
    }

    return doExecuteMethodForWidget(widget, wType, realMethod, args);
}

ArgsValidation ZeptoGramExecutor::validateArgsForWidget(WIDGET_TYPE wType, QList<QString> args)
{
    switch (wType)
    {
    case _CODE_INPUT: {
        if (args.isEmpty())
        {
            return ArgsValidation(-1, "Code should be provided for CODE_INPUT widget");
        }

        QString code = args.at(0);
        if (code.isNull() || code.isEmpty())
        {
            return ArgsValidation(-2, "Code for CODE_INPUT cannot be null or empty");
        }

        code = code.simplified().trimmed();

        if (code.length() != 5)
        {
            return ArgsValidation(-3, "Code length must be 5");
        }
        
        int digitsCount = 0;
        for (auto chr : code)
        {
            if (chr.isDigit()) {
                digitsCount++;
            }
        }

        if (digitsCount != code.length())
        {
            return ArgsValidation(-4, "Code should consists only from digits");
        }

        return ArgsValidation(0);
    }
    default: {
        return ArgsValidation(0);
    }
    }
}

ExecuteResult ZeptoGramExecutor::doExecuteMethodForWidget(QWidget* widget, WIDGET_TYPE wType, METHOD_NAME methodName, QList<QString> args)
{
    WidgetExecutor exec;

    switch (wType)
    {
    case BUTTON: {
        auto button = dynamic_cast<Ui::AbstractButton*>(widget);
        if (button == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::AbstractButton";
            return ExecuteResult::_false("Error converting widget to Ui::AbstractButton");
        }

        return exec.executeMethodOnButton(button, methodName);
    }
    case _RADIO_BUTTON: {
        /*auto button = (Ui::Radioenum<ProxyData::Settings>)widget
        if (button == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::Radioenum";
            return ExecuteResult::_false("Error converting widget to Ui::Radioenum");
        }*/

        return exec.executeMethodOnRadioButton(widget, methodName);
    }
    case _COUNTRY_INPUT: {
        auto cInput = dynamic_cast<CountryInput*>(widget);
        if (cInput == NULL)
        {
            qDebug() << "ZPT: Error converting widget to CountryInput";
            return ExecuteResult::_false("Error converting widget to CountryInput");
        }

        return exec.executeMethodOnCountryInput(cInput, methodName);
    }
    case SELECT: {
        auto mSelect = dynamic_cast<Ui::MultiSelect*>(widget);
        if (mSelect == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::MultiSelect";
            return ExecuteResult::_false("Error converting widget to Ui::MultiSelect");
        }

        return exec.executeMethodOnMultiSelect(mSelect, methodName);
    }
    case MASKED_INPUT_FIELD: {
        auto miField = dynamic_cast<Ui::MaskedInputField*>(widget);
        if (miField == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::MaskedInputField";
            return ExecuteResult::_false("Error converting widget to Ui::MaskedInputField");
        }

        return exec.executeMethodOnMaskedInputField(miField, methodName, args.isEmpty() ? QString("NO_STRING") : args.at(0));
    }
    case INPUT_FIELD: {
        auto iField = dynamic_cast<Ui::InputField*>(widget);
        if (iField == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::InputField";
            return ExecuteResult::_false("Error converting widget to Ui::InputField");
        }

        return exec.executeMethodOnInputField(iField, methodName, args.isEmpty() ? QString("NO_STRING") : args.at(0));
    }
    case LABEL: {
        auto label = dynamic_cast<Ui::FlatLabel*>(widget);
        if (label == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::FlatLabel";
            return ExecuteResult::_false("Error converting widget to Ui::FlatLabel");
        }

        return exec.executeMethodOnLabel(label, methodName, args.isEmpty() ? QString("NO_STRING") : args.at(0));
    }
    case _SCROLL_LIST: {
        if (widget == NULL)
        {
            qDebug() << "ZPT: Error converting widget to ?";
            return ExecuteResult::_false("Error converting widget to ?");
        }

        return exec.executeMethodOnScrollList(widget, methodName);
    }
    case _CODE_INPUT: {
        auto validationRes = validateArgsForWidget(wType, args);
        if (validationRes.result != 0)
        {
            return ExecuteResult::_false(validationRes.errorMessage);
        }

        auto codeInput = dynamic_cast<Ui::CodeInput*>(widget);
        if (codeInput == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::CodeInput";
            return ExecuteResult::_false("Error converting widget to Ui::CodeInput");
        }
        return exec.executeMethodOnCodeInput(codeInput, methodName, args.isEmpty() ? QString("NO_STRING") : args.at(0));
    }
    case _LAYER_WIDGET: {
        auto _layer = dynamic_cast<Ui::LayerStackWidget*>(widget);
        if (_layer == NULL)
        {
            qDebug() << "ZPT: Error converting widget to Ui::LayerStackWidget";
            return ExecuteResult::_false("Error converting widget to Ui::LayerStackWidget");
        }
        return exec.executeMethodOnLayerWidget(_layer, methodName);
    }
    case _SETTINGS_INFORMATION: {
        auto sett = dynamic_cast<Settings::Information*>(widget);
        if (sett == nullptr)
        {
            qDebug() << "ZPT: Error converting widget to Settings::Information";
            return ExecuteResult::_false("Error converting widget to Settings::Information");
        }
        return exec.executeMethodOnSettingsInformation(sett, methodName);
    }
    default:
        qDebug() << "ZPT: Not found";
        return ExecuteResult::_false("Not found");
    }
}

ExecuteResult ZeptoGramExecutor::executeCommand(const QString& command, QList<QString> args)
{
    QStringList cc = command.split(".");
    if(cc.size() < 2) {
        qDebug() << "ZPT: Incorrect command: " << command;
        return ExecuteResult::_false("Incorrect command");
       /* qDebug() << "ZPT: Executing external command: " << command;
        return executeExternalMethod(command);*/
    }

    QString widgetId = cc.at(0).simplified();
    WidgetAndType wt = this->findWidgetByName(widgetId);
    if (wt.isEmpty())
    {
        qDebug() << "ZPT: Not found widget with name: " << widgetId;
        return ExecuteResult::_false("Not found widget with name: " + widgetId);
    }

    QString method = cc.at(1);
    qDebug() << "ZPT: Executing internal method " + method << " on widget " << widgetId;
    return executeMethodForWidget(wt._widget, widgetId, wt._widgetType, method, args);
}

ExecuteResult ZeptoGramExecutor::selectChatByUsername(const QString& username)
{
    WidgetAndType wt = findWidgetByName(DIALOGS_WIDGET);
    if (wt._widget == nullptr)
    {
        return ExecuteResult::_false("Unknown error");
    }

    QWidget* w = wt._widget;
    Dialogs::InnerWidget* dw = dynamic_cast<Dialogs::InnerWidget*>(w);

    if (dw == nullptr)
    {
        return ExecuteResult::_false("Unknown error");
    }

    std::pair<QPoint, PeerId> dPosPeer = dw->getPointForChatWithUsername(username);
    QPoint pos = dPosPeer.first;
    if (pos.x() == -1)
    {
        QString errorMessage = "Not found any dialog for username: " + username;
        qDebug() << "ZPT: " << errorMessage;
        return ExecuteResult::_false(errorMessage);
    }

    PointClickOnWidget click;
    click(dw, pos);
    return ExecuteResult::_uint64(dPosPeer.second.value);
}

ExecuteResult ZeptoGramExecutor::selectChatByPeerId(const uint64& peerId)
{
    WidgetAndType wt = findWidgetByName(DIALOGS_WIDGET);
    if (wt._widget == nullptr)
    {
        return ExecuteResult::_false("Unknown error");
    }

    QWidget* w = wt._widget;
    Dialogs::InnerWidget* dw = dynamic_cast<Dialogs::InnerWidget*>(w);

    if (dw == nullptr)
    {
        return ExecuteResult::_false("Unknown error");
    }

    std::pair<QPoint, PeerId> dPosPeer = dw->getPointForChatWithPeerId(peerId);
    QPoint pos = dPosPeer.first;
    if (pos.x() == -1)
    {
        QString errorMessage = "Not found any dialog for peerId: " + peerId;
        qDebug() << "ZPT: " << errorMessage;
        return ExecuteResult::_false(errorMessage);
    }

    PointClickOnWidget click;
    click(dw, pos);
    return ExecuteResult::_uint64(dPosPeer.second.value);
}

void ZeptoGramExecutor::clickOnMessageWithText(uint64 peerId, const QString& messageText, bool leftButton, double widthDiffCoef)
{
    WidgetAndType wt = findWidgetByName(MAIN_DIALOG_HISTORY_WIDGET);
    if (wt._widget == nullptr)
    {
        return;
        //return ExecuteResult::_false("Unknown error");
    }

    QWidget* w = wt._widget;
    HistoryInner* hw = dynamic_cast<HistoryInner*>(w);
    if (hw == nullptr)
    {
        return;
    }

    QPoint messagePosition = hw->getPointOfMessageAndScroll(PeerId(peerId), messageText, widthDiffCoef);

    PointClickOnWidgetWithMouse click;
    click(hw, messagePosition, leftButton);
}

void ZeptoGramExecutor::clickOnInlineButtonWithText(int64 peerId, const QString& messageText, const QString& btnText,
    const bool leftButton, const double widthDiffCoef)
{
    WidgetAndType wt = findWidgetByName(MAIN_DIALOG_HISTORY_WIDGET);
    if (wt._widget == nullptr)
    {
        return;
        //return ExecuteResult::_false("Unknown error");
    }

    QWidget* w = wt._widget;
    HistoryInner* hw = dynamic_cast<HistoryInner*>(w);
    if (hw == nullptr)
    {
        return;
    }

    QPoint inlinePosition = hw->getPointOfInlineButtonMessageAndScroll(PeerId(peerId), messageText, btnText, widthDiffCoef);
    PointClickOnWidgetWithMouse click;
    click(hw, inlinePosition, leftButton);
}

std::vector<std::tuple<int32, uint64, QString>> ZeptoGramExecutor::getMessagesTextContaining(uint64 peerId, const QString& messageText)
{
    std::vector<std::tuple<int32, uint64, QString>> res;

    WidgetAndType wt = findWidgetByName(MAIN_DIALOG_HISTORY_WIDGET);
    if (wt._widget == nullptr)
    {
        return res;
        //return ExecuteResult::_false("Unknown error");
    }

    QWidget* w = wt._widget;
    HistoryInner* hw = dynamic_cast<HistoryInner*>(w);
    if (hw == nullptr)
    {
        return res;
    }

    res = hw->getMessagesTextContaining(PeerId(peerId), messageText);
    return res;
}

WidgetAndType ZeptoGramExecutor::findWidgetByName(QString name)
{
    if (registeredWidgets.contains(name))
    {
        return registeredWidgets.value(name);
    }

    return WidgetAndType();
}
