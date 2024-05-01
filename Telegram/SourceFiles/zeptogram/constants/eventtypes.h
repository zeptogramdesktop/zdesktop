#ifndef EVENTTYPES_H
#define EVENTTYPES_H

#include <QString>

namespace zeptogram
{
	struct OpenMainMenuEvent {};
	struct MainMenuOpenedEvent {};

	enum class ExternalMethodType {
		OPEN_MAIN_MENU = 0,
		CLOSE_MAIN_MENU = 1,
		OPEN_SETTINGS = 2,
		NONE = -1
	};
}

#endif // EVENTTYPES_H
