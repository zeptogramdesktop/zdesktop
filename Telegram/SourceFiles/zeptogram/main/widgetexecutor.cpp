#include "widgetexecutor.h"

#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>

#include "externalcommands.h"

using namespace zeptogram;

WidgetExecutor::WidgetExecutor(QObject *parent) : QObject{parent}
{
}

ExecuteResult WidgetExecutor::executeMethodOnButton(Ui::AbstractButton* button, METHOD_NAME methodName)
{
	switch (methodName)
	{
	case CLICK:
        doClickOnWidget(button);
        return _void();
    case IS_VISIBLE: {
        bool visible = button->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = button->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        button->setFocus();
        return _void();
    case CLEAR_FOCUS:
        button->clearFocus();
        return _void();
	default:
        return _bool(false);
	}
}

ExecuteResult WidgetExecutor::executeMethodOnRadioButton(QWidget * widget, METHOD_NAME methodName)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(widget);
        return _void();
    case IS_VISIBLE: {
        bool visible = widget->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = widget->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        widget->setFocus();
        return _void();
    case CLEAR_FOCUS:
        widget->clearFocus();
        return _void();
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnCountryInput(CountryInput* cInput, METHOD_NAME methodName)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(cInput);
        return _void();
    case IS_VISIBLE: {
        bool visible = cInput->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = cInput->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        cInput->setFocus();
        return _void();
    case CLEAR_FOCUS:
        cInput->clearFocus();
        return _void();
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnMultiSelect(Ui::MultiSelect* mSelect, METHOD_NAME methodName)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(mSelect);
        return _void();
    case IS_VISIBLE: {
        bool visible = mSelect->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = mSelect->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        mSelect->setFocus();
        return _void();
    case CLEAR_FOCUS:
        mSelect->clearFocus();
        return _void();
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnMaskedInputField(Ui::MaskedInputField* miField, METHOD_NAME methodName, const QString& text)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(miField);
        return _void();
    case IS_VISIBLE: {
        bool visible = miField->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = miField->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        miField->setFocus();
        return _void();
    case CLEAR_FOCUS:
        miField->clearFocus();
        return _void();
    case CLEAR:
        miField->clear();
        return _void();
    case SET_TEXT:
        miField->setText(text);
        return _void();
    case GET_TEXT: {
        const QString text = miField->getText();
        return _string(text);
    }
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnInputField(Ui::InputField* iField, METHOD_NAME methodName, const QString& text)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(iField);
        return _void();
    case IS_VISIBLE: {
        bool visible = iField->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = iField->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        iField->setFocus();
        return _void();
    case CLEAR_FOCUS:
        iField->clearFocus();
        return _void();
    case CLEAR:
        iField->clear();
        return _void();
    case SET_TEXT:
        iField->setText(text);
        return _void();
    case GET_TEXT: {
        const QString text = iField->getText();
        return _string(text);
    }
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnLabel(Ui::FlatLabel* label, METHOD_NAME methodName, const QString& text)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(label);
        return _void();
    case IS_VISIBLE: {
        bool visible = label->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = label->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        label->setFocus();
        return _void();
    case CLEAR_FOCUS:
        label->clearFocus();
        return _void();
    case SET_TEXT:
        label->setText(text);
        return _void();
    case GET_TEXT: {
        QString labelText = label->getText();
        return _string(labelText);
    }
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnScrollList(QWidget* scrollList, METHOD_NAME methodName)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(scrollList);
        return _void();
    case IS_VISIBLE: {
        bool visible = scrollList->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = scrollList->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        scrollList->setFocus();
        return _void();
    case CLEAR_FOCUS:
        scrollList->clearFocus();
        return _void();
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnCodeInput(CodeInput* codeInput, METHOD_NAME methodName, const QString& text)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(codeInput);
        return _void();
    case IS_VISIBLE: {
        bool visible = codeInput->isVisible();
        return _bool(visible);
    }
    case IS_HIDDEN: {
        bool hidden = codeInput->isHidden();
        return _bool(hidden);
    }
    case SET_FOCUS:
        codeInput->setFocus();
        return _void();
    case CLEAR_FOCUS:
        codeInput->clearFocus();
        return _void();
    case SET_TEXT:
        codeInput->setCode(text);
        return _void();
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnLayerWidget(LayerStackWidget* layer, METHOD_NAME methodName)
{
    switch (methodName)
    {
    case CLICK:
        doClickOnWidget(layer);
        return _void();
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::executeMethodOnSettingsInformation(Settings::Information* sett, METHOD_NAME methodName)
{
    switch (methodName)
    {
    case GET_PHONE: {
        const QString p = sett->getPhoneNumber();
        return _string(p);
    }
    default:
        return _bool(false);
    }
}

ExecuteResult WidgetExecutor::_void()
{
    ExecuteResult res;
    res.setType(ExecuteResult::_VOID);
    return res;
}

ExecuteResult WidgetExecutor::_bool(bool value)
{
    QJsonObject obj;
    obj["value"] = value;
    ExecuteResult res;
    res.setType(ExecuteResult::_BOOL);
    res.setResult(obj);
    return res;
}

ExecuteResult WidgetExecutor::_string(const QString& value)
{
    QJsonObject obj;
    obj["value"] = value;
    ExecuteResult res;
    res.setType(ExecuteResult::_STRING);
    res.setResult(obj);
    return res;
}

void WidgetExecutor::doClickOnWidget(QWidget* widget)
{
    ClickOnWidget click;
    click(widget);

    /*QSize widgetSize = widget->geometry().size();

    double x = (double)widgetSize.width() / 2;
    double y = (double)widgetSize.height() / 2;

    QPoint localPos = QPoint(x, y);
    QPoint windowPos = widget->mapToGlobal(localPos);
    QPoint screenPos = widget->mapToGlobal(localPos);


    QMouseEvent* pressEvent = new QMouseEvent(QEvent::MouseButtonPress, localPos, windowPos, screenPos,
        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QMouseEvent* releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, localPos, windowPos, screenPos,
        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    (void)QApplication::instance()->sendEvent(widget, pressEvent);
    QApplication::processEvents();

    (void)QApplication::instance()->sendEvent(widget, releaseEvent);
    QApplication::processEvents();*/
}
