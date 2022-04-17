QtAV code taken at April 2022 from revision #fdc613dc99304f208cff0bb25b3ded14bb993237

https://github.com/wang-bin/QtAV

See the story for bugzilla: https://bugs.kde.org/show_bug.cgi?id=448681

Code Review with plenty of fixes and improvements:

- Compilation rules ported to clean Cmake with usual Find scripts for dependencies.
  See core/cmake/rules/RulesMediaPlayer.cmake
- Fix of all GCC 11 warnings.
- Fixes Qt Signal/Slots.
- Port code to pure Qt5/Qt6.
* Fixes header guards.
* Fixes includes rules from Qt and local.
* Apply fixes for cppcheck static analyzer.
* Apply fixes for Krazy static analyzer.
* Apply fixes for Clang-Scan static analyzer.
* Apply fixes for Clazy static analyzer.
* Apply fixes for Coverity-Scan static analyzer.
* Use qCDebug() with digiKam debug spaces.

TODO:

- Use cmake to find libuchardet dependency.
- Use cmake to find DirectX dependencies (Windows only).
- Use cmake to find CUDA dependency.
- Use cmake to find OpenSL dependency.
- Port to FFMPEG 5.
