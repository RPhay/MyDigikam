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

    - Hide all private class and structure symbols with Q_DECL_HIDDEN macro.

    - Fix all Clang warnings under MacOS.

    - Fix all MSVC warnings under Windows.

    - Fixes Qt Signal/Slots connexions syntax.

    - Port code to pure Qt5/Qt6.

    - Remove Qt4 rules.

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

    - Remove support of Qt version < 5.9.

    - Merge QtAV/src/ and QtAV/widgets/ together.

    - Fix API docs.

    - Fix header guards.

    - Use Clang-tidy to patch code with nullptr instead NULL or 0.

    - Apply fixes for Krazy static analyzer.

    - Apply fixes for cppcheck static analyzer.

    - Apply fixes for Clang-Scan static analyzer.

    - Fix comments formating everywhere.

    - Fix includes rules/order from Qt and local.

    - Fix coding style (brackets, tabs to spaces, indents, if logics, init pointers, etc.).

    - Port GLWidgetRenderer class to QOpenGLWidget for Qt5 and Qt6
      GLWidgetRenderer2 still based on Qt5::QGLWidget and is disabled for Qt6.

    - Apply fixes for Coverity-Scan static analyzer.

    - Fix all undefined warnings at compilation time.

    - Port to FFMPEG 5 API with backward compatibility with FFMPEG 4.

        - See the patch from https://github.com/wang-bin/QtAV/issues/1377#issuecomment-1014959481
        - Use Cmake to check if the libavcodec version >=59 (FFMPEG 5 API).
        - All FFMPEG 5 API are wrapped with pre-processor rules like this:

        #ifndef HAVE_FFMPEG_VERSION5
            /* FFMpeg version 4 codes. */
        #else
            /* FFMpeg version 5 codes. */
        #endif

        - FFMPEG 4 API codes still unchanged.

    - Add config widgets to setup video and audio backends.

* Under Progress:

    - Apply fixes for Clazy static analyzer.
    - Perform regression tests with FFMPEG 5.

* TODO:

    - FFMPEG 5 API and VAAPI support: FFMPEG4::vaapi struct is removed and code must be ported to
                                      AVCodecContext.hw_frames_ctx instead. Code is currently disabled.

    - FFMPEG 5 API and CUDA support : FFMPEG4::AVBitStreamFilterContext struct is removed
                                      (aka FFMPEG4::FF_API_OLD_BSF define). Port to new API.


* Unmaintained Features:

    - IOS support.

    - CdeArv Arm chip video decoding support.

* Notes:

    - Only the Desktop features are maintained (MacOS, Linux, and Windows).

    - Compilation are processed with Continuous Integration : MSVC, FreeBSD, Suse Linux.

    - Compilation are processed with Continuous deployement : Clang, MSVC, MinGW, and GCC.

    - Stand alone media player originaly located from examples/player are move and compiled to core/avplayer.

    - Original codes from tests and examples sub-directories are moved and compiled into core/tests/video/qtav.
