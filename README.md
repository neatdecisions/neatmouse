# NeatMouse
Keyboard-based mouse emulator for Windows operating systems.

## Build
The repository includes Visual Studio project which could be built in Visual Studio 2015 Community Edition.

You may want to downgrade Platform Toolset if building in the earlier Visual Studio version. Currently it is set to `v140_xp` to preserve compatibility with Windows XP.

## Translations
Source file for the translations: `neatmouse\[misc]\NeatMouse_translations.xml`

Translations can be edited using [Locale Editor](https://github.com/neatdecisions/locale-editor), and then exported to `neatmouse\NeatMouseWtl\res\langs` folder (one `lng`-file per language).

## Installer
InnoSetup is used to generate an installer for NeatMouse. The script is located at `neatmouse\[misc]\neatmouse.iss`. It requires `NeatMouse.exe` to be present in `neatmouse\NeatMouseWtl\Release` folder.
