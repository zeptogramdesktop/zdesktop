# Zeptogram Desktop - First Telegram Desktop version with REST API to control UI Elements

## Features

- REST API server built in Telegram
- [python API client](https://github.com/zeptogramdesktop/zeptogramapi)
- UI elements have ids
- pywinauto support
- Mouse and Keyboard free while interacting through REST API - it means you can minimize Zeptogram window while your control script is running

## Usecase

The most valuable usecase is when you need to automate some operations with your account(s) already authorized in Telegram Desktop and don't want to manually login by code into account via API:

- Run Zeptigram client with an existing tdata folder
- Run Selenium scripts to open [web telegram](https://web.telegram.org/k/) in a browser
- Login via Selenium scripts
- Receive 2FA code for login and get it from Zeptogram API
- Continue operations in web version having saved local storage session in a browser (it means you can restart your browser and continue operations without re-authoring again)

Why not use official API?
You still need to manually enter 2FA code when authorizing using official API at first. Zeptogram client provides API to read the 2FA code

### Libraries used

- [qt-httpserver](https://github.com/petrpopov/qt-httpserver)
- [magic_enum](https://github.com/Neargye/magic_enum)

## Build instructions (same as for official client)

* Windows [(32-bit)][win32] [(64-bit)][win64]
* [macOS][mac]
* [GNU/Linux using Docker][linux]

[//]: # (LINKS)
[telegram]: https://telegram.org
[telegram_desktop]: https://desktop.telegram.org
[telegram_api]: https://core.telegram.org
[telegram_proto]: https://core.telegram.org/mtproto
[license]: LICENSE
[win32]: docs/building-win.md
[win64]: docs/building-win-x64.md
[mac]: docs/building-mac.md
[linux]: docs/building-linux.md
[preview_image]: https://github.com/telegramdesktop/tdesktop/blob/dev/docs/assets/preview.png "Preview of Telegram Desktop"
[preview_image_url]: https://raw.githubusercontent.com/telegramdesktop/tdesktop/dev/docs/assets/preview.png
