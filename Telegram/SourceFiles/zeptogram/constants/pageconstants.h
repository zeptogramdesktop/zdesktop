#ifndef PAGECONSTANTS_H
#define PAGECONSTANTS_H

#include <list>
#include <string>

namespace zeptogram {
	// pages
	constexpr auto PAGE_NAME_START_STEP = "START_STEP";
	constexpr auto PAGE_NAME_QR_STEP = "QR_STEP";
	constexpr auto PAGE_NAME_PHONE_STEP = "PHONE_STEP";
	constexpr auto PAGE_NAME_CODE_STEP = "CODE_STEP";
	constexpr auto PAGE_NAME_PASSWORD_STEP = "PASSWORD_STEP";
	constexpr auto PAGE_NAME_SIGNUP_STEP = "SIGNUP_STEP";
	constexpr auto PAGE_NAME_MAIN_WINDOW = "MAIN_PAGE";

	
	const std::list<std::string> PAGES({ 
		PAGE_NAME_START_STEP, 
		PAGE_NAME_QR_STEP,
		PAGE_NAME_PHONE_STEP, 
		PAGE_NAME_CODE_STEP, 
		PAGE_NAME_PASSWORD_STEP, 
		PAGE_NAME_SIGNUP_STEP,
		PAGE_NAME_MAIN_WINDOW
	});

	// widgets
	constexpr auto DIALOGS_WIDGET = "dialogsWidget";

	// intro start
	constexpr auto TITLE_LABEL = "titleLabel"; // intro_widget.h
	constexpr auto DESCRIPTION_LABEL = "descriptionLabel"; // intro_widget.h

	constexpr auto NEXT_BUTTON = "nextButton"; // intro_widget.h
	constexpr auto BACK_BUTTON = "backButton"; // intro_widget.h
	constexpr auto SETTINGS_BUTTON = "settingsButton"; // intro_widget.h
	constexpr auto CHANGE_LANGUAGE_BUTTON = "changeLanguageButton"; // intro_widget.cpp
	constexpr auto CLOSE_SETTINGS_BUTTON = "closeSettingsButton"; //settings_intro.cpp
	constexpr auto COUNTRY_CLOSE_BUTTON = "countryCloseButton"; // country_select_box.cpp
	constexpr auto LOGIN_PHONE_BUTTON = "loginViaPhoneButton"; // intro_qr.cpp
	constexpr auto LOGIN_QR_BUTTON = "loginViaQrButton"; // intro_phone.cpp

	constexpr auto COUNTRY_INPUT = "countryInput"; // intro_widget.h

	constexpr auto SEARCH_INPUT = "searchInputField"; // multi_select.cpp
	constexpr auto SCROLL_LIST = "scrollList"; // multi_select.cpp
	constexpr auto PHONE_CODE_INPUT = "phoneCodeMaskedInputField"; // intro_phone.cpp
	constexpr auto PHONE_NUMBER_INPUT = "phoneNumberMaskedInputField"; // intro_phone.cpp
	constexpr auto CODE_INPUT = "codeInput"; // intro_code.cpp
	constexpr auto PASSWORD_INPUT = "passwordMaskedInputField"; // intro_password_check.cpp
	// intro end

	// main window start
	constexpr auto MAIN_MENU_BUTTON = "mainMenuButton"; // dialogs_widget.cpp
	constexpr auto MAIN_SEARCH_INPUT = "mainSearchInputField"; // dialogs_widget.cpp
	constexpr auto MAIN_LAYER_WIDGET = "mainLayerWidget";
	// main window end
	
	// main menu start
	constexpr auto MAIN_MENU_SETTINGS_BUTTON = "mainMenuSettingsButton"; // window_main_menu.cpp
	constexpr auto MAIN_MENU_SAVED_MESSAGES_BUTTON = "mainMenuSavedMessagesButton"; // window_main_menu.cpp

	// settings dialog
	constexpr auto ACCOUNT_INFO_BUTTON = "accountInfoButton"; // settings_main.cpp
	constexpr auto PRIVACY_SEC_BUTTON = "privacyAndSecurityButton"; // settings_main.cpp
	constexpr auto ADVANCED_BUTTON = "advancedButton"; // settings_main.cpp
	constexpr auto CLOSE_DIALOG_X_BUTTON = "closeDialogXButton"; // info_wrap_widget.cpp
	constexpr auto BACK_DIALOG_ARROW_BUTTON = "backDialogArrowButton"; // info_top_bar.cpp

	// settings dialog - edit BIO
	constexpr auto BIO_INPUT_FIELD = "bioInputField";
	constexpr auto NAME_EDIT_BUTTON = "nameEditButton"; // settings_information.cpp
	constexpr auto USERNAME_EDIT_BUTTON = "usernameEditButton"; // settings_information.cpp
	constexpr auto PHONE_EDIT_BUTTON = "phoneEditButton"; // settings_information.cpp
	constexpr auto PHONE_LABEL = "phoneLabel"; // settings_information.cpp
	constexpr auto SETTINGS_INFO_SECTION = "settingsInfoSection"; // settings_information.cpp

	// settings dialog - edit account info
	constexpr auto FIRST_NAME_INPUT_FIELD = "firstNameInputField"; // add_contact_box.cpp
	constexpr auto LAST_NAME_INPUT_FIELD = "lastNameInputField"; // add_contact_box.cpp

	// edit username box
	constexpr auto USERNAME_EDIT_CANCEL_BUTTON = "usernameEditCancelButton"; // username_box.cpp
	constexpr auto USERNAME_EDIT_SAVE_BUTTON = "usernameEditSaveButton"; // username_box.cpp
	constexpr auto USERNAME_EDIT_USERNAME_INPUT = "usernameEditUsernameInput"; // username_box.cpp
	constexpr auto USERNAME_EDIT_CHECK_LABEL = "usernameEditCheckLabel"; // username_box.cpp
	constexpr auto USERNAME_EDIT_DESCRIPTION_LABEL = "usernameEditDescriptionLabel"; // username_box.cpp

	// save edit box
	constexpr auto SAVE_DIALOG_BUTTON = "saveDialogButton"; // add_contact_box.cpp
	constexpr auto CANCEL_DIALOG_BUTTON = "cancelDialogButton"; // add_contact_box.cpp

	// settings dialog - edit Two-Step Verification
	constexpr auto TWO_STEP_VER_BUTTON = "twoStepVerificationButton"; // settings_privacy_security.cpp
	constexpr auto TWO_STEP_VERIFICATION_ABOUT_LABEL = "twoStepVerificationAboutLabel"; // settings_cloud_password_common.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_INTRO_BUTTON = "twoStepCreatePasswordIntroButton"; // settings_cloud_password_start.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_ENTER_PASSWORD_INPUT = "twoStepCreatePasswordEnterPasswordInputField"; // settings_cloud_password_input.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_RE_ENTER_PASSWORD_INPUT = "twoStepCreatePasswordReEnterPasswordInputField"; // settings_cloud_password_input.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_ERROR_LABEL = "twoStepCreatePasswordErrorLabel"; // settings_cloud_password_input.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_HINT_LABEL = "twoStepCreatePasswordHintLabel"; // settings_cloud_password_input.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_CONTINUE_BUTTON = "twoStepCreatePasswordContinueButton"; // settings_cloud_password_input.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_HINT_INPUT = "twoStepCreatePasswordHintInput"; // settings_cloud_password_hint.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_HINT_ERROR_LABEL = "twoStepCreatePasswordHintErrorLabel"; // settings_cloud_password_hint.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_HINT_SKIP_BUTTON = "twoStepCreatePasswordHintSkipButton"; // settings_cloud_password_hint.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_HINT_CONTINUE_BUTTON = "twoStepCreatePasswordHintContinueButton"; // settings_cloud_password_hint.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_RECOVERY_EMAIL_INPUT = "twoStepCreatePasswordRecoveryEmailInput"; // settings_cloud_password_email.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_RECOVERY_EMAIL_SKIP_BUTON = "twoStepCreatePasswordRecoveryEmailSkipButton"; // settings_cloud_password_email.cpp
	constexpr auto TWO_STEP_CREATE_PASSWORD_RECOVERY_EMAIL_SAVE_AND_FINISH_BUTON = "twoStepCreatePasswordRecoveryEmailSaveAndFinishButton"; // settings_cloud_password_email.cpp

	// settings dialog - edit advanced proxy
	constexpr auto CONNECTION_TYPE_BUTTON = "connectionTypeButton"; // settings_advanced.cpp
	constexpr auto DISABLE_PROXY_BUTTON = "disableProxyRadioButton"; // connection_box.cpp
	constexpr auto USE_SYSTEM_PROXY_BUTTON = "useSystemProxyRadioButton"; // connection_box.cpp
	constexpr auto CUSTOM_PROXY_RADIO_BUTTON = "customProxyRadioButton"; // connection_box.cpp
	constexpr auto ADD_PROXY_CONFIRM_BUTTON = "addProxyConfirmButton"; // connection_box.cpp
	constexpr auto ADD_PROXY_CLOSE_BUTTON = "addProxyCloseButton"; // connection_box.cpp

	// settings dialog - connection type - add proxy form
	constexpr auto SAVE_PROXY_BUTTON = "saveProxyButton"; // connection_box.cpp
	constexpr auto CANCEL_PROXY_BUTTON = "cancelProxyButton"; // connection_box.cpp
	constexpr auto SHARE_PROXY_BUTTON = "shareProxyButton"; // connection_box.cpp
	constexpr auto PROXY_HOSTNAME_INPUT = "proxyHostnameInput"; // connection_box.cpp
	constexpr auto PROXY_PORT_INPUT = "proxyPortInput"; // connection_box.cpp
	constexpr auto PROXY_SECRET_INPUT = "proxySecretInput"; // connection_box.cpp
	constexpr auto PROXY_USERNAME_INPUT = "proxyUsernameInput"; // connection_box.cpp
	constexpr auto PROXY_PASSWORD_INPUT = "proxyPasswordInput"; // connection_box.cpp
	constexpr auto PROXY_TYPE_SOCKS5_RADIO_BUTTON = "proxyTypeSocks5RadioButton"; // connection_box.cpp
	constexpr auto PROXY_TYPE_HTTP_RADIO_BUTTON = "proxyTypeHttpRadioButton"; // connection_box.cpp
	constexpr auto PROXY_TYPE_MTPROTO_RADIO_BUTTON = "proxyTypeMtprotoRadioButton"; // connection_box.cpp

	// small confirm box with two buttons and warning
	constexpr auto CONFIRM_BOX_LABEL = "confirmBoxLabel"; // confirm_box.cpp
	constexpr auto CONFIRM_BOX_CANCEL_BUTTON = "confirmBoxCancelButton"; // confirm_box.cpp
	constexpr auto CONFIRM_BOX_CONFIRM_BUTTON = "confirmBoxConfirmButton"; // confirm_box.cpp

	// change and manage password
	constexpr auto CHANGE_PASSWORD_BUTTON = "changePasswordButton"; // settings_cloud_password_manage.cpp
	constexpr auto SET_OR_CHANGE_EMAIL_BUTTON = "setOrChangeEmailButton"; // settings_cloud_password_manage.cpp
	constexpr auto DISABLE_CLOUD_PASSWORD_BUTTON = "disableCloudPasswordButton"; // settings_cloud_password_manage.cpp


	// main dialog window
	constexpr auto MAIN_DIALOG_INPUT_FIELD = "mainDialogInputField"; // history_widget.cpp
	constexpr auto MAIN_DIALOG_SEND_BUTTON = "mainDialogSendButton"; // history_widget.cpp
	constexpr auto MAIN_DIALOG_START_BOT_BUTTON = "mainDialogStartBotButton"; // history_widget.cpp
	constexpr auto MAIN_DIALOG_HISTORY_WIDGET = "mainDialogHistoryWidget"; // history_widget.cpp

	// main chat side panel
	constexpr auto CLOSE_SIDE_X_BUTTON = "closeSideXButton"; // info_wrap_widget.cpp

	// main chat top bar
	constexpr auto MAIN_CHAT_INFO_TOGGLE_BUTTON = "mainChatInfoToggleButton"; // history_view_top_bar_widget.cpp
	constexpr auto MAIN_CHAT_BACK_BUTTON = "mainChatBackButton"; // history_view_top_bar_widget.cpp
	constexpr auto MAIN_CHAT_JOIN_CHANNEL_BUTTON = "mainChatJoinChannelButton"; // history_view_top_bar_widget.cpp
	constexpr auto MAIN_CHAT_MUTE_UNMUTE_CHANNEL_BUTTON = "mainChatMuteUnmuteChannelButton"; // history_view_top_bar_widget.cpp

	// bot webview
	constexpr auto BOT_WEBVIEW_CLOSE_BUTTON = "botWebViewCloseButton"; // separate_panel.cpp

	const std::list<std::string> WIDGETS({
		TITLE_LABEL,
		DESCRIPTION_LABEL,

		NEXT_BUTTON,
		BACK_BUTTON,
		SETTINGS_BUTTON,
		CHANGE_LANGUAGE_BUTTON,
		CLOSE_SETTINGS_BUTTON,
		COUNTRY_CLOSE_BUTTON,
		LOGIN_PHONE_BUTTON,
		LOGIN_QR_BUTTON,

		COUNTRY_INPUT,

		SEARCH_INPUT,
		SCROLL_LIST,
		PHONE_CODE_INPUT,
		PHONE_NUMBER_INPUT,
		CODE_INPUT,
		PASSWORD_INPUT,

		MAIN_MENU_BUTTON,
		MAIN_SEARCH_INPUT,
		MAIN_MENU_SETTINGS_BUTTON,

		MAIN_MENU_SAVED_MESSAGES_BUTTON,

		ACCOUNT_INFO_BUTTON,
		PRIVACY_SEC_BUTTON,
		ADVANCED_BUTTON,
		CLOSE_DIALOG_X_BUTTON,
		BACK_DIALOG_ARROW_BUTTON,

		BIO_INPUT_FIELD,
		NAME_EDIT_BUTTON,
		USERNAME_EDIT_BUTTON,
		PHONE_EDIT_BUTTON,
		PHONE_LABEL,
		SETTINGS_INFO_SECTION,

		FIRST_NAME_INPUT_FIELD,
		LAST_NAME_INPUT_FIELD,
		SAVE_DIALOG_BUTTON,
		CANCEL_DIALOG_BUTTON,

		TWO_STEP_VER_BUTTON,
		TWO_STEP_VERIFICATION_ABOUT_LABEL,
		TWO_STEP_CREATE_PASSWORD_INTRO_BUTTON,
		TWO_STEP_CREATE_PASSWORD_ENTER_PASSWORD_INPUT,
		TWO_STEP_CREATE_PASSWORD_RE_ENTER_PASSWORD_INPUT,
		TWO_STEP_CREATE_PASSWORD_ERROR_LABEL,
		TWO_STEP_CREATE_PASSWORD_HINT_LABEL,
		TWO_STEP_CREATE_PASSWORD_CONTINUE_BUTTON,
		TWO_STEP_CREATE_PASSWORD_HINT_INPUT,
		TWO_STEP_CREATE_PASSWORD_HINT_ERROR_LABEL,
		TWO_STEP_CREATE_PASSWORD_HINT_SKIP_BUTTON,
		TWO_STEP_CREATE_PASSWORD_HINT_CONTINUE_BUTTON,
		TWO_STEP_CREATE_PASSWORD_RECOVERY_EMAIL_INPUT,
		TWO_STEP_CREATE_PASSWORD_RECOVERY_EMAIL_SKIP_BUTON,
		TWO_STEP_CREATE_PASSWORD_RECOVERY_EMAIL_SAVE_AND_FINISH_BUTON,

		CONNECTION_TYPE_BUTTON,
		DISABLE_PROXY_BUTTON,
		USE_SYSTEM_PROXY_BUTTON,
		CUSTOM_PROXY_RADIO_BUTTON,
		ADD_PROXY_CONFIRM_BUTTON,
		ADD_PROXY_CLOSE_BUTTON,

		SAVE_PROXY_BUTTON,
		CANCEL_PROXY_BUTTON,
		SHARE_PROXY_BUTTON,
		PROXY_HOSTNAME_INPUT,
		PROXY_PORT_INPUT,
		PROXY_SECRET_INPUT,
		PROXY_USERNAME_INPUT,
		PROXY_PASSWORD_INPUT,
		PROXY_TYPE_SOCKS5_RADIO_BUTTON,
		PROXY_TYPE_HTTP_RADIO_BUTTON,
		PROXY_TYPE_MTPROTO_RADIO_BUTTON,

		CONFIRM_BOX_LABEL,
		CONFIRM_BOX_CANCEL_BUTTON,
		CONFIRM_BOX_CONFIRM_BUTTON,

		CHANGE_PASSWORD_BUTTON,
		SET_OR_CHANGE_EMAIL_BUTTON,
		DISABLE_CLOUD_PASSWORD_BUTTON,

		USERNAME_EDIT_CANCEL_BUTTON,
		USERNAME_EDIT_SAVE_BUTTON,
		USERNAME_EDIT_USERNAME_INPUT,
		USERNAME_EDIT_CHECK_LABEL,
		USERNAME_EDIT_DESCRIPTION_LABEL,
		
		MAIN_DIALOG_INPUT_FIELD,
		MAIN_DIALOG_SEND_BUTTON,
		MAIN_DIALOG_START_BOT_BUTTON,

		CLOSE_SIDE_X_BUTTON,

		MAIN_CHAT_INFO_TOGGLE_BUTTON,
		MAIN_CHAT_BACK_BUTTON,
		MAIN_CHAT_JOIN_CHANNEL_BUTTON,
		MAIN_CHAT_MUTE_UNMUTE_CHANNEL_BUTTON,

		BOT_WEBVIEW_CLOSE_BUTTON
	});
} // namespace zeptogram
#endif // PAGECONSTANTS_H
