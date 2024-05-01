#ifndef WIDGETEXECUTOR_H
#define WIDGETEXECUTOR_H

#include <QObject>

#include "ui/abstract_button.h"
#include "ui/widgets/buttons.h"
#include "ui/countryinput.h"
#include "ui/widgets/multi_select.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/fields/special_fields.h"
#include "ui/widgets/labels.h"
#include "ui/layers/layer_widget.h"
#include "ui/rp_widget.h"
#include "intro/intro_code_input.h"
#include "ui/widgets/checkbox.h"
#include "mtproto/mtproto_proxy_data.h"
#include "settings/settings_information.h"

#include "zeptogram/constants/widgettypes.h"
#include "zeptogram/main/executeresult.h"

using namespace zeptogram;
using namespace Ui;
using ProxyData = MTP::ProxyData;

class WidgetExecutor : public QObject
{
    Q_OBJECT
public:
    explicit WidgetExecutor(QObject *parent = nullptr);

    ExecuteResult executeMethodOnButton(Ui::AbstractButton* button, METHOD_NAME methodName);
    ExecuteResult executeMethodOnRadioButton(QWidget* widget, METHOD_NAME methodName);
    ExecuteResult executeMethodOnCountryInput(CountryInput* cInput, METHOD_NAME methodName);
    ExecuteResult executeMethodOnMultiSelect(Ui::MultiSelect* mSelect, METHOD_NAME methodName);
    ExecuteResult executeMethodOnMaskedInputField(Ui::MaskedInputField* miField, METHOD_NAME methodName, const QString& text);
    ExecuteResult executeMethodOnInputField(Ui::InputField* iField, METHOD_NAME methodName, const QString& text);
    ExecuteResult executeMethodOnLabel(Ui::FlatLabel* label, METHOD_NAME methodName, const QString& text);
    ExecuteResult executeMethodOnScrollList(QWidget* scrollList, METHOD_NAME methodName);
    ExecuteResult executeMethodOnCodeInput(CodeInput* codeInput, METHOD_NAME methodName, const QString& text);
    ExecuteResult executeMethodOnLayerWidget(LayerStackWidget* layer, METHOD_NAME methodName);
    ExecuteResult executeMethodOnSettingsInformation(Settings::Information* sett, METHOD_NAME methodName);

private:
    ExecuteResult _void();
    ExecuteResult _bool(bool value);
    ExecuteResult _string(const QString& value);
    void doClickOnWidget(QWidget* widget);
};

#endif // WIDGETEXECUTOR_H
