QtAV code taken at April 2022 from revision #fdc613dc99304f208cff0bb25b3ded14bb993237

https://github.com/wang-bin/QtAV

See the story for bugzilla: https://bugs.kde.org/show_bug.cgi?id=448681

Code review with plenty of fixes and improvements:

- Compilation rules ported to clean CMake with usual Find scripts for dependencies.
  See core/cmake/rules/RulesMediaPlayer.cmake
- Fix of all GCC 11 warnings.
- Fixes Qt Signal/Slots.
- Port code to pure Qt5/Qt6.
- Use qCDebug()/qCWarning categories with digiKam debug spaces. Drop Logger class.
- Use KDE i18n() instead tr().
- Use cmake to find libuchardet dependency.
* Fix header guards.
* Fix includes rules/order from Qt and local.
* Fix coding style.
* Use nullptr instead NULL or 0.
* Apply fixes for cppcheck static analyzer.
* Apply fixes for Krazy static analyzer.
* Apply fixes for Clang-Scan static analyzer.
* Apply fixes for Clazy static analyzer.
* Apply fixes for Coverity-Scan static analyzer.

UNMAINTAINED FEATURES:

- OpenSL-ES support.
- WinRT support.
- Android support.

TODO:

- Use cmake to find DirectX dependencies (Windows only).
- Use cmake to find CUDA Toolkit dependency.
- Port to FFMPEG 5.
