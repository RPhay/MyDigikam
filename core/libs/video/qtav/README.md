QtAV code taken at April 2022 from revision #fdc613dc99304f208cff0bb25b3ded14bb993237

https://github.com/wang-bin/QtAV

See the story for bugzilla: https://bugs.kde.org/show_bug.cgi?id=448681

Code review with plenty of fixes and improvements:

x Compilation rules ported to clean CMake with usual Find scripts for dependencies.
  See core/cmake/rules/RulesMediaPlayer.cmake and files in core/cmake/modules.
x Fix all GCC 11 warnings.
x Fixes Qt Signal/Slots.
x Port code to pure Qt5/Qt6.
x Use qCDebug()/qCWarning categories with digiKam debug spaces. Drop Logger class.
x Use KDE i18n() instead tr().
x Use cmake to find libuchardet dependency.
x Add SSE2 and SSE4.1 support with CMake.
x Remove QML support.
x Fix compilation under native Windows without DirectX (OpenGL).
x Fix compilation under FreeBSD.
x Use cmake to find X11 and XV dependencies.
* Fix API docs.
* Remove support of Qt version < 5.
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

- OpenSL-ES support (non-opensource toolkit).
- WinRT support (used for Windows Phone only).
- Android support.
- IOS support.

TODO:

* Use cmake to find DirectX and GDI+ dependencies (Native Windows only):
  x xaudio2
  - direct2d
  - dxva
  x d3d11va
  x dsound
  x gdiplus

+ Use cmake to find CUDA Toolkit dependencies.
+ Port to FFMPEG 5 API with backward compatibility.
