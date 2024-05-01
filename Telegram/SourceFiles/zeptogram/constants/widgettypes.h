#ifndef WIDGETTYPES_H
#define WIDGETTYPES_H

#include <QMap>
#include "zeptogram/util/EnumReflection.h"

namespace zeptogram {
	Z_ENUM_NS(WIDGET_TYPE,
		BUTTON = 0,
		SELECT = 1,
		MASKED_INPUT_FIELD = 2,
		INPUT_FIELD = 3,
		_COUNTRY_INPUT = 4,
		LABEL = 5,
		_SCROLL_LIST = 6,
		_CODE_INPUT = 7,
		_LAYER_WIDGET = 8,
		_DIALOGS_WIDGET = 9,
		_HISTORY_WIDGET = 10,
		_RADIO_BUTTON = 11,
		_SETTINGS_INFORMATION = 12,
		NONE
	)

	static const WIDGET_TYPE WIDGET_TYPES[] = { BUTTON, SELECT, MASKED_INPUT_FIELD, INPUT_FIELD, _COUNTRY_INPUT, LABEL, _SCROLL_LIST, _CODE_INPUT, _LAYER_WIDGET, _DIALOGS_WIDGET, _HISTORY_WIDGET, _RADIO_BUTTON, _SETTINGS_INFORMATION };

	Z_ENUM_NS(METHOD_NAME,
		CLICK = 0,
		SET_TEXT = 1,
		GET_TEXT = 2,
		CLEAR = 3,
		SET_FOCUS = 4,
		CLEAR_FOCUS = 5,
		IS_VISIBLE = 6,
		IS_HIDDEN = 7,
		GET_PHONE = 8
	)

	Z_ENUM_NS(METHOD_RESULT_TYPE,
		_VOID = 0,
		_STRING = 1,
		_BOOL = 2
	)

	static const QMap<QString, QList<METHOD_NAME>> AVAILABLE_METHODS {
		{QString::number(BUTTON), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN }},
		{QString::number(SELECT), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN } },
		{QString::number(MASKED_INPUT_FIELD), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN, SET_TEXT, GET_TEXT, CLEAR } },
		{QString::number(INPUT_FIELD), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN, SET_TEXT, GET_TEXT, CLEAR } },
		{QString::number(_COUNTRY_INPUT), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN } },
		{QString::number(LABEL), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN, SET_TEXT, GET_TEXT } },
		{QString::number(_SCROLL_LIST), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN } },
		{QString::number(_CODE_INPUT), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN, SET_TEXT } },
		{QString::number(_LAYER_WIDGET), { CLICK } },
		{QString::number(_RADIO_BUTTON), { CLICK, SET_FOCUS, CLEAR_FOCUS, IS_VISIBLE, IS_HIDDEN } },
		{QString::number(_SETTINGS_INFORMATION), { GET_PHONE } }
	};
}

#endif // WIDGETTYPES_H
