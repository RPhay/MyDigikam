QtAV code taken at April 2022 from revision #fdc613dc99304f208cff0bb25b3ded14bb993237

https://github.com/wang-bin/QtAV

See the story for bugzilla: https://bugs.kde.org/show_bug.cgi?id=448681

Code review with plenty of fixes and improvements:

- [DONE] Compilation rules ported to improved and simplified CMake :

         - Use Find scripts to resolve dependencies.
         - See core/cmake/rules/RulesMediaPlayer.cmake
         - See files in core/cmake/modules.

- [DONE] Fix all GCC 11 warnings.

- [DONE] Fixes Qt Signal/Slots.

- [DONE] Port code to pure Qt5/Qt6.

- [DONE] Use qCDebug()/qCWarning categories with digiKam debug spaces.

- [DONE] Drop Logger class.

- [DONE] Use KDE i18n() instead tr().

- [DONE] Use cmake to find libuchardet dependency.

- [DONE] Add SSE2 and SSE4.1 support with CMake.

- [DONE] Remove QML support.

- [DONE] Remove IPP compiler suport.

- [DONE] Fix compilation under native Windows without DirectX (OpenGL).

- [DONE] Fix compilation under FreeBSD.

- [DONE] Use cmake to find X11 and XV dependencies.

- [DONE] Re-organize class files from QtAVWidgets by categories.

- [DONE] Re-organize class files from QtAV by categories.

- [DONE] Drop VDA support:

         - QTAV_HAVE_VDA=1 : codec/video/VideoDecoderVDA.cpp
         - FFMpeg::libavcodec/vda.h have been dropped from version 4.0 in favor of VideoToolbox Apple Framework.
         - See Changelog : https://github.com/FFmpeg/FFmpeg/blob/16463520d9de05481320303dd49ea5158c715c9f/Changelog#L310

- [DONE] Use cmake to find DirectX and GDI+ dependencies (Native Windows only):

         - xaudio2
         - direct2d
         - d3d11va
         - dsound
         - gdiplus
         - dxva:

            - QTAV_HAVE_DXVA=1 : codec/video/VideoDecoderDXVA.cpp
            - Depends of FFMPEG::libavcodec/dxva2.h
            - See implementation : https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/dxva2.h

- [DONE] Port to pure Qt5 LTS and Qt6

- [DONE] Fix broken compilation under MinGW (MXE)

- [DONE] Use cmake to find CUDA Toolkit dependencies ==> no needs, CUDA is always loaded dynamically.

- [DONE] Remove support of Qt version < 5.

- [DONE] Fix API docs.

- [DONE] Fix header guards.

- [PROG] Fix comments formating everywhere.

- [PROG] Fix includes rules/order from Qt and local.

- [PROG] Fix coding style.

- [PROG] Use nullptr instead NULL or 0.

- [PROG] Apply fixes for cppcheck static analyzer.

- [PROG] Apply fixes for Krazy static analyzer.

- [PROG] Apply fixes for Clang-Scan static analyzer.

- [PROG] Apply fixes for Clazy static analyzer.

- [PROG] Apply fixes for Coverity-Scan static analyzer.

- [TODO] Port to FFMPEG 5 API with backward compatibility with FFMPEG 4.

UNMAINTAINED FEATURES:

- OpenSL-ES support (non-opensource toolkit).

- Android support.

- IOS support.

- Arm support.

NOTES:

- Only the Desktop features are maintained (MacOS, Linux, and Windows).

- Compilation are processed with Continuous Integration : MSVC, FreeBSD, Suse Linux.

- Compilation are processed with Continuous deployement : MacOS CLang, Windows MinGW (MXE), Linux GCC.
