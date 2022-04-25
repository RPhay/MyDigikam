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
x Remove IPP compiler suport.
x Fix compilation under native Windows without DirectX (OpenGL).
x Fix compilation under FreeBSD.
x Use cmake to find X11 and XV dependencies.
x Re-organize class files from QtAVWidgets by categories.
x Re-organize class files from QtAV by categories.
x Drop VDA support: QTAV_HAVE_VDA=1 : codec/video/VideoDecoderVDA.cpp
  FFMpeg::libavcodec/vda.h have been dropped from version 4.0 in favor of VideoToolbox Apple Framework.
  https://github.com/FFmpeg/FFmpeg/blob/16463520d9de05481320303dd49ea5158c715c9f/Changelog#L310
x Use cmake to find DirectX and GDI+ dependencies (Native Windows only):
  x xaudio2
  x direct2d
  x d3d11va
  x dsound
  x gdiplus
  x dxva        ==> QTAV_HAVE_DXVA=1 : codec/video/VideoDecoderDXVA.cpp
                    Depends of FFMpeg::libavcodec/dxva2.h
                    https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/dxva2.h
x Port to pure Qt5 LTS and Qt6

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
- Arm support.

TODO:


+ Use cmake to find CUDA Toolkit dependencies.
+ Port to FFMPEG 5 API with backward compatibility.
