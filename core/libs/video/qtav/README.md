QtAV code taken at April 2022 from revision #fdc613dc99304f208cff0bb25b3ded14bb993237

https://github.com/wang-bin/QtAV

See the story for bugzilla: https://bugs.kde.org/show_bug.cgi?id=448681

Code review with plenty of fixes and improvements. See the list of tasks below.

* Done:

    - Compilation rules ported, improved, and simplified with Cmake :

      - Drop all Qmake rules.
      - Use Find scripts to resolve dependencies.
      - See core/cmake/rules/RulesMediaPlayer.cmake
      - See files in core/cmake/modules.

    - Fix all GCC 11 warnings.

    - Fix all Clang warnings under MacOS.

    - Fixes Qt Signal/Slots connexions syntax.

    - Port code to pure Qt5/Qt6.

    - Use qCDebug()/qCWarning categories with digiKam debug spaces.

    - Drop Logger class.

    - Use KDE i18n() instead tr().

    - Use cmake to find libuchardet dependency.

    - Add SSE2 and SSE4.1 support with CMake.

    - Remove QML support.

    - Remove IPP compiler suport.

    - Fix compilation under native Windows without DirectX (OpenGL).

    - Fix compilation under FreeBSD.

    - Use cmake to find X11 and XV dependencies.

    - Re-organize class files from QtAVWidgets by categories.

    - Re-organize class files from QtAV by categories.

    - Drop VDA support:

      - QTAV_HAVE_VDA=1 : codec/video/VideoDecoderVDA.cpp
      - FFMpeg::libavcodec/vda.h have been dropped from version 4.0 in favor of VideoToolbox Apple Framework.
      - See Changelog : https://github.com/FFmpeg/FFmpeg/blob/16463520d9de05481320303dd49ea5158c715c9f/Changelog#L310

    - Use Cmake to find OpenSL-ES dependency.

    - Use Cmake to find DirectX and GDI+ dependencies with Native Windows and MinGW:

      - xaudio2
      - direct2d
      - d3d11va
      - dsound
      - gdiplus
      - dxva:

         - QTAV_HAVE_DXVA=1 : codec/video/VideoDecoderDXVA.cpp
         - Depends of FFMPEG::libavcodec/dxva2.h
         - See implementation : https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/dxva2.h

    - Port to pure Qt5 LTS and Qt6

    - Fix broken compilation under MinGW (MXE)

    - Use cmake to find CUDA Toolkit dependencies ==> no needs, CUDA is always loaded dynamically.

    - Remove support of Qt version < 5.

    - Fix API docs.

    - Fix header guards.

    - Use Clang-tidy to patch code with nullptr instead NULL or 0.

    - Apply fixes for Krazy static analyzer.

* Under Progress:

    - Fix comments formating everywhere.

    - Fix includes rules/order from Qt and local.

    - Fix coding style (brackets, tabs to spaces, indents, etc.).

    - Apply fixes for cppcheck static analyzer.

    - Apply fixes for Clang-Scan static analyzer.

    - Apply fixes for Clazy static analyzer.

    - Apply fixes for Coverity-Scan static analyzer.

* TODO:

    - Port to FFMPEG 5 API with backward compatibility with FFMPEG 4.

    - Add config widgets to setup video and audio backends.

    - Remove GLWidgetRenderer class and port GLWidgetRenderer2 to Qt6::QOpenGLWidget.

* Unmaintained Features:

    - IOS support.

    - Arm support.

* Notes:

    - Only the Desktop features are maintained (MacOS, Linux, and Windows).

    - Compilation are processed with Continuous Integration : MSVC, FreeBSD, Suse Linux.

    - Compilation are processed with Continuous deployement : MacOS CLang, Windows MinGW (MXE), Linux GCC.

