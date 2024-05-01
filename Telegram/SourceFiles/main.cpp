/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "core/launcher.h"
#include "zeptogram/server/zeptogramserver.h"

int main(int argc, char *argv[]) {
	ZeptoGramServer* server = new ZeptoGramServer();

	const auto launcher = Core::Launcher::Create(argc, argv, server);
	return launcher ? launcher->exec() : 1;
}
