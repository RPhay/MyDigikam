#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2015      by Mohamed_Anwer, <m_dot_anwer at gmx dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# ==============================================================================

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Check dependencies for ${PROJECT_NAME} version ${DIGIKAM_VERSION_STRING}")
message(STATUS "")

set(KF5_MIN_VERSION       "5.5.0")
set(QT_MIN_VERSION        "5.9.0")
set(EXIV2_MIN_VERSION     "0.27.0")
set(OPENCV_MIN_VERSION    "3.3.0")
set(LENSFUN_MIN_VERSION   "0.2.6.0")      # For Lens auto-correction plugin
set(QTAV_MIN_VERSION      "1.12.0")       # For video support.
set(KSANE_MIN_VERSION     "5.5.0")        # For digital scanner support.
set(CALENDAR_MIN_VERSION  "5.5.0")        # Calendar Core dependency for plugin.
set(AKONADI_MIN_VERSION   "4.89.0")       # Akonadi Contact dependency.

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

# ==============================================================================

option(ENABLE_KFILEMETADATASUPPORT       "Build digiKam with KDE files indexer support (default=OFF)"                         OFF)
option(ENABLE_AKONADICONTACTSUPPORT      "Build digiKam with KDE Mail Contacts support (default=OFF)"                         OFF)
option(ENABLE_MEDIAPLAYER                "Build digiKam with Media Player support (default=OFF)"                              OFF)
option(ENABLE_DBUS                       "Build digiKam with DBUS support (default=ON)"                                       ON)
option(ENABLE_APPSTYLES                  "Build digiKam with support for changing the widget application style (default=OFF)" OFF)
option(ENABLE_QWEBENGINE                 "Build digiKam with QWebEngine instead of QWebKit (default=ON)"                      ON)

# Mysql support options (experimental):
option(ENABLE_MYSQLSUPPORT               "Build digiKam with MySQL dabatase support (default=ON)"                             ON)
option(ENABLE_INTERNALMYSQL              "Build digiKam with internal MySQL server executable (default=ON)"                   ON)

# Developer options:
option(ENABLE_DIGIKAM_MODELTEST          "Enable ModelTest on some models for debugging (default=OFF)"                        OFF)
option(ENABLE_SANITIZERS                 "Enable ASAN and UBSAN sanitizers when available (default=OFF)"                      OFF)
option(BUILD_WITH_CCACHE                 "Use ccache to speed up compilations"                                                OFF)

# Packaging options:
if(MINGW)
    option(ENABLE_DRMINGW                "Enable the Dr. MinGW crash handler support for windows (default ON)"                ON)
    option(ENABLE_MINGW_HARDENING_LINKER "Enable DEP (NX), ASLR, and high-entropy ASLR linker flags for MinGW (default ON)"   ON)
endif()

############## Find Packages ###################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED
             NO_MODULE COMPONENTS
             Core
             Concurrent
             Widgets
             Gui
             Sql
             Xml
             PrintSupport
             Network
)

if(ENABLE_QWEBENGINE)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS WebEngineWidgets)
else()
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS WebKitWidgets)
endif()

find_package(Qt5 ${QT_MIN_VERSION}
             OPTIONAL_COMPONENTS
             DBus
             OpenGL
             XmlPatterns                # For Rajce plugin
)

if(ENABLE_DBUS)
    if(NOT Qt5DBus_FOUND)
        set(ENABLE_DBUS OFF)
    endif()
endif()

# Dependencies For unit tests and CLI test tools

if(BUILD_TESTING)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED
                 NO_MODULE COMPONENTS
                 Test
    )

    find_package(Qt5 ${QT_MIN_VERSION}
                 QUIET
                 OPTIONAL_COMPONENTS
                 Qml WebView  # Optional, for 'sialis' O2 library test tool.
    )
endif()

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED
             COMPONENTS
             XmlGui
             CoreAddons
             Config
             Service
             WindowSystem
             Solid
             I18n
)

find_package(KF5 ${KF5_MIN_VERSION} QUIET
             OPTIONAL_COMPONENTS
             KIO                    # For Desktop integration (Widgets only).
             IconThemes             # For Desktop integration.
             FileMetaData           # For KDE file indexer support.
             ThreadWeaver           # For Panorama tool.
             NotifyConfig           # KDE desktop application notify configuration.
             Notifications          # KDE desktop notifications integration.
)

find_package(KF5 ${AKONADI_MIN_VERSION} QUIET
             OPTIONAL_COMPONENTS
             AkonadiContact         # For KDE Mail Contacts support.
             Contacts               # API for contacts/address book data.
)

find_package(KF5 ${KSANE_MIN_VERSION} QUIET
             OPTIONAL_COMPONENTS
             Sane                   # For digital scanner support.
)

find_package(KF5 ${CALENDAR_MIN_VERSION} QUIET
             OPTIONAL_COMPONENTS
             CalendarCore           # For Calendar tool.
)

if ("${KF5CalendarCore_VERSION}" VERSION_GREATER 5.6.40)
    set(HAVE_KCALENDAR_QDATETIME TRUE)
endif()

if(ENABLE_AKONADICONTACTSUPPORT AND (NOT KF5AkonadiContact_FOUND OR NOT KF5Contacts_FOUND))
    set(ENABLE_AKONADICONTACTSUPPORT OFF)
endif()

if(ENABLE_KFILEMETADATASUPPORT AND NOT KF5FileMetaData_FOUND)
    set(ENABLE_KFILEMETADATASUPPORT OFF)
endif()

# Check if KIO have been compiled with KIOWidgets. digiKam only needs this one.
if(KF5KIO_FOUND)
    get_target_property(KIOWidgets_INCLUDE_DIRS KF5::KIOWidgets
                        INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "KF5::KIOWidgets include dirs: ${KIOWidgets_INCLUDE_DIRS}")

    if(NOT KIOWidgets_INCLUDE_DIRS)
        message(STATUS "KF5::KIOWidgets not available in shared KIO library. KIO support disabled.")
        set(KF5KIO_FOUND FALSE)
    endif()
endif()

# ==============================================================================
# Dependencies Rules

# mandatory

DETECT_JPEG()
set(DIGIKAM_LIBJPEG_DIR libjpeg/${JPEG_LIB_VERSION})
message(STATUS "Using libjpeg sub-directory: ${DIGIKAM_LIBJPEG_DIR}")

find_package(TIFF)
find_package(PNG)
find_package(Boost)
find_package(LCMS2)
find_package(EXPAT)    # For DNGWriter: XMP SDK need Expat library to compile.
find_package(Threads)  # For DNGWriter and LibRaw which needs native threads support.
find_package(X265)     # For HEIF encoding support.

if(NOT X265_FOUND)
    set(X265_LIBRARIES "")
endif()

find_package(exiv2 REQUIRED)

set_package_properties("exiv2"     PROPERTIES
                       DESCRIPTION "Required to build digiKam"
                       URL         "https://www.exiv2.org"
                       TYPE        RECOMMENDED
                       PURPOSE     "Library to manage image metadata"
)

if("${exiv2_VERSION}" VERSION_LESS ${EXIV2_MIN_VERSION})
    message(FATAL_ERROR "Exiv2 version is too old (${exiv2_VERSION})! Minimal version required:${EXIV2_MIN_VERSION}.")
endif()

# -- check Media player --------------------------------------------------------

find_package(FFmpeg COMPONENTS AVCODEC AVDEVICE AVFILTER AVFORMAT AVUTIL SWSCALE)
find_package(QtAV)

if(ENABLE_MEDIAPLAYER)

    if(${AVCODEC_FOUND} AND ${AVDEVICE_FOUND} AND ${AVFILTER_FOUND} AND ${AVFORMAT_FOUND} AND ${AVUTIL_FOUND} AND ${SWSCALE_FOUND})

        include_directories(${FFMPEG_INCLUDE_DIRS})

    else()

        set(ENABLE_MEDIAPLAYER OFF)
        set(FFMPEG_FOUND OFF)
        message(STATUS "ENABLE_MEDIAPLAYER option is enabled but FFMpeg cannot be found. Media player support is disabled.")

    endif()

    if(NOT ${QtAV_FOUND} OR "${QTAV_VERSION_STRING}" VERSION_LESS ${QTAV_MIN_VERSION})

        set(ENABLE_MEDIAPLAYER OFF)
        set(QtAV_FOUND OFF)
        message(STATUS "ENABLE_MEDIAPLAYER option is enabled but QtAV cannot be found. Media player support is disabled.")

    else()

        include_directories(${QTAV_INCLUDE_DIRS})

    endif()

    if (${QtAV_FOUND} AND ${FFMPEG_FOUND})

        message(STATUS "Media player support is enabled.")

    endif()

endif()


# -- check OpenCV --------------------------------------------------------------

DETECT_OPENCV(${OPENCV_MIN_VERSION} core objdetect imgproc imgcodecs dnn flann)

if(${OpenCV_FOUND})
    if(${OpenCV_VERSION} VERSION_LESS ${OPENCV_MIN_VERSION})
        message(STATUS "OpenCV < ${OPENCV_MIN_VERSION} have been found. Please use a more recent version.")
        set(OpenCV_FOUND FALSE)
    endif()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/app/utils/digikam_opencv.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/app/utils/digikam_opencv.h)

# -- optionals -----------------------------------------------------------------

find_package(FLEX)                                 # For Panorama tool.
find_package(BISON)                                # For Panorama tool.

find_package(LibXslt)                              # For HTMLGallery tool.
find_package(LibXml2)                              # For HTMLGallery tool.

find_package(Marble)                               # For geolocation support.

find_package(PkgConfig)
find_package(Jasper)                               # For JPEG 2000 support.
find_package(Eigen3)                               # For Refocus tool.

# -- ImageMagick rules --------------------------------------------------------

find_package(ImageMagick COMPONENTS Magick++ MagickCore)

if(ImageMagick_Magick++_FOUND)
    add_definitions(${ImageMagick_Magick++_DEFINITIONS})
endif()

# -- X11 rules ----------------------------------------------------------------

# For Monitor Profiles management with LCMS
find_package(X11)

if(X11_FOUND)
    find_package(Qt5 ${QT_MIN_VERSION} NO_MODULE COMPONENTS X11Extras)
    set(HAVE_X11 TRUE)
else()
    set(HAVE_X11 FALSE)
endif()

# -- OpenGL rules -------------------------------------------------------------

if(POLICY CMP0072)
    # Prefers Legacy OpenGL by default when available introduced in CMake version 3.11
    # Details: https://cmake.org/cmake/help/git-stage/policy/CMP0072.html
    cmake_policy(SET CMP0072 OLD)
endif()

find_package(OpenGL)                               # For Presentation tool.

message(STATUS "OpenGL found;               ${OPENGL_FOUND}")
message(STATUS "OpenGL GLU extension found; ${OPENGL_GLU_FOUND}")
message(STATUS "OpenGL Qt5 found;           ${Qt5OpenGL_FOUND}")

# decide if Presentation tool can be built with OpenGL
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND Qt5OpenGL_FOUND)
    set(HAVE_OPENGL TRUE)
elseif()
    set(HAVE_OPENGL FALSE)
endif()

# -- Windows rules ------------------------------------------------------------

# For LibRaw
if(WIN32)
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
endif()

if(MINGW)

    # NOTE: inspired from https://phabricator.kde.org/T3917
    if(ENABLE_MINGW_HARDENING_LINKER)

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--dynamicbase -Wl,--nxcompat -Wl,--disable-auto-image-base")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--dynamicbase -Wl,--nxcompat -Wl,--disable-auto-image-base")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--dynamicbase -Wl,--nxcompat -Wl,--disable-auto-image-base")

        if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

            # Enable high-entropy ASLR for 64-bit
            # The image base has to be >4GB for HEASLR to be enabled.
            # The values used here are kind of arbitrary.
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--image-base,0x140000000")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--image-base,0x180000000")
            set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--image-base,0x180000000")

        endif()

        message(STATUS "MinGW linker Security Flags enabled")

    else()

        message(WARNING "MinGW Linker Security Flags not enabled!")

    endif ()

    # Enable C++11 support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

endif()

# -- To link under Solaris (see bug #274484) -----------------------------------

if(NOT WIN32)
    find_library(MATH_LIBRARY m)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
    find_library(KVM_LIBRARY kvm)
endif()

# ==============================================================================
# More Optional Dependencies

find_package(Doxygen)

if(NOT CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
    find_package(Lqr-1)
endif()

if(BUILD_WITH_CCACHE)
    message(STATUS "Looking for ccache...")
    find_program(CCACHE_FOUND ccache)
    message(STATUS "ccache program found: ${CCACHE_FOUND}...")
endif()

# -- libgphoto2 rules ----------------------------------------------------------

if(NOT WIN32)

    find_package(Gphoto2)

    if(Gphoto2_FOUND)

        if("${GPHOTO2_VERSION_STRING}" VERSION_GREATER "2.4.0")
            set(VERSION_GPHOTO2 true)
        else()
            set(VERSION_GPHOTO2 false)
        endif()

        if("${GPHOTO2_VERSION_STRING}" VERSION_GREATER "2.5.0")
            set(VERSION_GPHOTO25 true)
            message(STATUS "libgphoto2 API version >= 2.5")
        else()
            set(VERSION_GPHOTO25 false)
            message(STATUS "libgphoto2 API version < 2.5")
        endif()

        if(VERSION_GPHOTO25)
            set(HAVE_GPHOTO25 1)
        else()
            set(HAVE_GPHOTO25 0)
        endif()

    endif()

endif()

# -- Check LensFun library for Lens auto-correction tool -----------------------

find_package(LensFun)

if(LENSFUN_VERSION)
    message(STATUS "liblensfun: Found version ${LENSFUN_VERSION} (required: ${LENSFUN_MIN_VERSION})")
    if(${LENSFUN_VERSION} VERSION_LESS ${LENSFUN_MIN_VERSION})
        set(LensFun_FOUND FALSE)
    endif()
else()
    message(STATUS "liblensfun: Version information not found, your version is probably too old.")
    set(LensFun_FOUND FALSE)
endif()

# -- Check dependencies for Panorama tool --------------------------------------

if(FLEX_FOUND AND BISON_FOUND AND KF5ThreadWeaver_FOUND)
    set(HAVE_PANORAMA 1)
else()
    set(HAVE_PANORAMA 0)
endif()

# -- Check dependencies for HTMLGallery tool -----------------------------------

if(LibXml2_FOUND AND LibXslt_FOUND)
    set(HAVE_HTMLGALLERY 1)
else()
    set(HAVE_HTMLGALLERY 0)
endif()

# -- Check dependencies for libraw ---------------------------------------------

message(STATUS "")
message(STATUS "--------------------------------------------------")
message(STATUS "digiKam RawEngine dependencies checks:")
message(STATUS "")

include(MacroOpenMP)
DETECT_OPENMP()

if(OPENMP_FOUND)

    message(STATUS "RawEngine will be compiled with OpenMP support")

else()

    # to force libraw to use OpenMP with Macports and MXE.
    if (APPLE OR MINGW)

        add_definitions(-DLIBRAW_FORCE_OPENMP)
        set(OPENMP_FOUND ON)
        message(STATUS "RawEngine will be compiled with OpenMP support")

    else()

        message(STATUS "RawEngine will not be compiled with OpenMP support")

    endif()

endif()

# Flag to use libjasper with LibRaw RedCine codec

if(Jasper_FOUND)

    add_definitions(-DUSE_JASPER)
    include_directories(${JASPER_INCLUDE_DIR})
    message(STATUS "RawEngine will be compiled with RedCine codec")

else()

    message(STATUS "RawEngine will not be compiled with RedCine codec")

endif()

# JPEG library check

message(STATUS "Looking for LibJpeg")

if(JPEG_FOUND)

    # JPEG lib version is done on top level through MacroJPEG.cmake

    if (${JPEG_LIB_VERSION} LESS 80)

        set(JPEG8_FOUND FALSE)

    else()

        set(JPEG8_FOUND TRUE)

    endif()

endif()

# Flag to use libjpeg with LibRaw DNG lossy codec

if(JPEG8_FOUND)

    add_definitions(-DUSE_JPEG)
    add_definitions(-DUSE_JPEG8)
    include_directories(${JPEG_INCLUDE_DIR})
    message(STATUS "RawEngine will be compiled with DNG lossy codec")

else()

    message(STATUS "RawEngine will not be compiled with DNG lossy codec")

endif()

message(STATUS "Looking for PThreads")
set(PTHREADS_FOUND (CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT))

# Registration of Libraw configuration to a dedicated header

MACRO_BOOL_TO_01(JPEG8_FOUND  LIBRAW_USE_DNGLOSSYCODEC)
MACRO_BOOL_TO_01(OPENMP_FOUND LIBRAW_USE_OPENMP)
MACRO_BOOL_TO_01(Jasper_FOUND LIBRAW_USE_REDCINECODEC)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libs/rawengine/libraw/libraw_config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/libs/rawengine/libraw/libraw_config.h)

# -- Debug Symbols rules under MacOS -------------------------------------------

MACOS_DEBUG_POLICIES()

# -- compilation options definitions -------------------------------------------

message(STATUS "--------------------------------------------------")
message(STATUS "")

MACRO_BOOL_TO_01(KF5Sane_FOUND                HAVE_KSANE)
MACRO_BOOL_TO_01(KF5FileMetaData_FOUND        HAVE_KFILEMETADATA)
MACRO_BOOL_TO_01(KF5CalendarCore_FOUND        HAVE_KCALENDAR)
MACRO_BOOL_TO_01(KF5Notifications_FOUND       HAVE_KNOTIFICATIONS)
MACRO_BOOL_TO_01(KF5NotifyConfig_FOUND        HAVE_KNOTIFYCONFIG)
MACRO_BOOL_TO_01(KF5KIO_FOUND                 HAVE_KIO)
MACRO_BOOL_TO_01(KF5IconThemes_FOUND          HAVE_KICONTHEMES)
MACRO_BOOL_TO_01(LensFun_FOUND                HAVE_LENSFUN)
MACRO_BOOL_TO_01(Lqr-1_FOUND                  HAVE_LIBLQR_1)
MACRO_BOOL_TO_01(Gphoto2_FOUND                HAVE_GPHOTO2)
MACRO_BOOL_TO_01(Jasper_FOUND                 HAVE_JASPER)
MACRO_BOOL_TO_01(Eigen3_FOUND                 HAVE_EIGEN3)
MACRO_BOOL_TO_01(Marble_FOUND                 HAVE_MARBLE)
MACRO_BOOL_TO_01(ENABLE_AKONADICONTACTSUPPORT HAVE_AKONADICONTACT)
MACRO_BOOL_TO_01(ENABLE_MYSQLSUPPORT          HAVE_MYSQLSUPPORT)
MACRO_BOOL_TO_01(ENABLE_INTERNALMYSQL         HAVE_INTERNALMYSQL)
MACRO_BOOL_TO_01(ENABLE_MEDIAPLAYER           HAVE_MEDIAPLAYER)
MACRO_BOOL_TO_01(ENABLE_DBUS                  HAVE_DBUS)
MACRO_BOOL_TO_01(ENABLE_APPSTYLES             HAVE_APPSTYLE_SUPPORT)
MACRO_BOOL_TO_01(ENABLE_QWEBENGINE            HAVE_QWEBENGINE)
MACRO_BOOL_TO_01(ENABLE_DRMINGW               HAVE_DRMINGW)
MACRO_BOOL_TO_01(ImageMagick_Magick++_FOUND   HAVE_IMAGE_MAGICK)
MACRO_BOOL_TO_01(X265_FOUND                   HAVE_X265)
MACRO_BOOL_TO_01(Qt5XmlPatterns_FOUND         HAVE_QTXMLPATTERNS)
MACRO_BOOL_TO_01(CCACHE_FOUND                 HAVE_CCACHE)

# Set config files accordingly with optional dependencies

configure_file(app/utils/digikam_config.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/app/utils/digikam_config.h)

# ==============================================================================
# Log messages

message(STATUS "")
message(STATUS "----------------------------------------------------------------------------------")
message(STATUS " digiKam ${DIGIKAM_VERSION_STRING} dependencies results   <https://www.digikam.org>")
message(STATUS "")

PRINT_COMPONENT_COMPILE_STATUS("MySQL Database Support"  ENABLE_MYSQLSUPPORT)
PRINT_COMPONENT_COMPILE_STATUS("MySQL Internal Support"  ENABLE_INTERNALMYSQL)
PRINT_COMPONENT_COMPILE_STATUS("DBUS Support"            ENABLE_DBUS)
PRINT_COMPONENT_COMPILE_STATUS("App. Style Support"      ENABLE_APPSTYLES)
PRINT_COMPONENT_COMPILE_STATUS("QWebEngine Support"      ENABLE_QWEBENGINE)

# ==============================================================================

PRINT_LIBRARY_STATUS("libboostgraph" "https://www.boost.org/doc/libs"                    "(version >= 1.43.0)"                Boost_FOUND)
PRINT_LIBRARY_STATUS("libexiv2"      "https://www.exiv2.org"                             "(version >= ${EXIV2_MIN_VERSION}"   exiv2_FOUND)
PRINT_LIBRARY_STATUS("libexpat"      "https://libexpat.github.io"                        "(version >= 2.0.0)"                 EXPAT_FOUND)
PRINT_LIBRARY_STATUS("libjpeg"       "https://github.com/libjpeg-turbo/libjpeg-turbo"    "(version >= 6b)"                    JPEG_FOUND)
PRINT_LIBRARY_STATUS("libkde"        "https://www.kde.org"                               "(version >= ${KF5_MIN_VERSION})"    KF5_FOUND)
PRINT_LIBRARY_STATUS("liblcms"       "https://github.com/mm2/Little-CMS"                 "(version >= 2.0.0)"                 LCMS2_FOUND)
PRINT_LIBRARY_STATUS("libopencv"     "https://opencv.org"                                "(version >= ${OPENCV_MIN_VERSION})" OpenCV_FOUND)
PRINT_LIBRARY_STATUS("libpng"        "https://libpng.sourceforge.io/"                    "(version >= 1.2.7)"                 PNG_FOUND)
PRINT_LIBRARY_STATUS("libpthread"    "https://www.gnu.org/software/hurd/libpthread.html" "(version >= 2.0.0)"                 CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
PRINT_LIBRARY_STATUS("libqt"         "https://www.qt.io"                                 "(version >= ${QT_MIN_VERSION})"     Qt5_FOUND)
PRINT_LIBRARY_STATUS("libtiff"       "https://gitlab.com/libtiff/libtiff/"               "(version >= 3.8.2)"                 TIFF_FOUND)

# ==============================================================================

PRINT_OPTIONAL_LIBRARY_STATUS("bison"             "https://www.gnu.org/software/bison/"                             "(version >= 2.5.0)"                    "digiKam will be compiled without Panorama support."                          BISON_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("doxygen"           "https://github.com/doxygen/doxygen"                              "(version >= 1.8.0)"                    "digiKam will be compiled without API documentation building support."        Doxygen_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("ccache"            "https://ccache.dev"                                              "(version >= 3.0.0)"                    "digiKam will be compiled without CCACHE build support."                      HAVE_CCACHE)
PRINT_OPTIONAL_LIBRARY_STATUS("flex"              "https://github.com/westes/flex"                                  "(version >= 2.5.0)"                    "digiKam will be compiled without Panorama support."                          FLEX_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libakonadicontact" "https://cgit.kde.org/akonadi-contacts.git/about/"                "(version >= ${AKONADI_MIN_VERSION})"   "digiKam will be compiled without KDE desktop address book support."          KF5AkonadiContact_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libmagick++"       "https://www.imagemagick.org"                                     "(version >= 6.7.0)"                    "digiKam will be compiled without ImageMagick codecs."                        HAVE_IMAGE_MAGICK)
PRINT_OPTIONAL_LIBRARY_STATUS("libeigen3"         "https://github.com/eigenteam/eigen-git-mirror"                   "(version >= 3.0.0)"                    "digiKam will be compiled without Refocus tool support."                      Eigen3_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libgphoto2"        "https://github.com/gphoto"                                       "(version >= 2.4.0)"                    "digiKam will be compiled without GPhoto2 camera drivers support."            Gphoto2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libjasper"         "https://github.com/mdadams/jasper"                               "(version >= 1.7.0)"                    "digiKam will be compiled without JPEG2000 support."                          Jasper_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkcalendarcore"  "https://cgit.kde.org/kcalendarcore.git/about/"                   "(version >= ${CALENDAR_MIN_VERSION})"  "digiKam will be compiled without advanced calendar support."                 KF5CalendarCore_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkfilemetadata"  "https://cgit.kde.org/kfilemetadata.git/about/"                   "(version >= ${KF5_MIN_VERSION})"       "digiKam will be compiled without KDE desktop file metadata support."         KF5FileMetaData_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkiconthemes"    "https://cgit.kde.org/kiconthemes.git/about/"                     "(version >= ${KF5_MIN_VERSION})"       "digiKam will be compiled without KDE desktop icon themes support."           KF5IconThemes_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkio"            "https://cgit.kde.org/kio.git/about/"                             "(version >= ${KF5_MIN_VERSION})"       "digiKam will be compiled without KDE desktop integration support."           KF5KIO_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libknotifications" "https://cgit.kde.org/knotifyconfig.git/about/"                   "(version >= ${KF5_MIN_VERSION})"       "digiKam will be compiled without KDE desktop notifications support."         KF5Notifications_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libknotifyconfig"  "https://cgit.kde.org/knotifications.git/about/"                  "(version >= ${KF5_MIN_VERSION})"       "digiKam will be compiled without KDE desktop notify configuration support."  KF5NotifyConfig_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libksane"          "https://cgit.kde.org/libksane.git/about/"                        "(version >= ${KSANE_MIN_VERSION})"     "digiKam will be compiled without flat scanners support."                     KF5Sane_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("liblensfun"        "https://lensfun.sourceforge.net"                                 "(version >= 0.2.6)"                    "digiKam will be compiled without Lens Auto Correction tool support."         LensFun_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("liblqr-1"          "https://liblqr.wikidot.com"                                      "(version >= 0.4.1)"                    "digiKam will be compiled without Contents Aware Resizer tool support."       Lqr-1_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libmarble"         "https://cgit.kde.org/marble.git/about/"                          "(version >= 0.22.0)"                   "digiKam will be compiled without geolocation maps support."                  Marble_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libqtav"           "https://www.qtav.org/"                                           "(version >= ${QTAV_MIN_VERSION})"      "digiKam will be compiled without Media Player support."                      QtAV_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libthreadweaver"   "https://cgit.kde.org/threadweaver.git/about/"                    "(version >= ${KF5_MIN_VERSION})"       "digiKam will be compiled without Panorama support."                          KF5ThreadWeaver_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libxml2"           "https://xmlsoft.org"                                             "(version >= 2.7.0)"                    "digiKam will be compiled without HTMLGallery support."                       LibXml2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libxslt"           "https://xmlsoft.org/XSLT"                                        "(version >= 1.1.0)"                    "digiKam will be compiled without HTMLGallery support."                       LibXslt_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libx265"           "https://x265.org"                                                "(version >= 2.2)"                      "digiKam will be compiled without HEIF encoding support."                     X265_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("OpenGL"            "https://www.mesa3d.org"                                          "(version >= 11.0.0)"                   "digiKam will be compiled without OpenGL support."                            HAVE_OPENGL)
PRINT_OPTIONAL_LIBRARY_STATUS("libqtxmlpatterns"  "https://www.qt.io"                                               "(version >= ${QT_MIN_VERSION})"        "digiKam will be compiled without Rajce tool support."                        HAVE_QTXMLPATTERNS)

# ==============================================================================

if(Boost_FOUND                          AND
   exiv2_FOUND                          AND
   EXPAT_FOUND                          AND
   JPEG_FOUND                           AND
   KF5_FOUND                            AND
   LCMS2_FOUND                          AND
   OpenCV_FOUND                         AND
   PNG_FOUND                            AND
   Qt5_FOUND                            AND
   TIFF_FOUND                           AND
   Threads_FOUND                        AND
   (Qt5Test_FOUND OR NOT BUILD_TESTING) AND
   (Qt5X11Extras_FOUND OR NOT HAVE_X11) AND
   (CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
  )
    message(STATUS " digiKam can be compiled.................. YES")
    set(DIGIKAM_CAN_BE_COMPILED true)
else()
    message(FATAL_ERROR " digiKam will be compiled.................. NO  (See the README file for more details about dependencies)")
    set(DIGIKAM_CAN_BE_COMPILED false)
endif()

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "")

if(DIGIKAM_CAN_BE_COMPILED)

    include(MacroGitHeader)
    GIT_HEADER()

    include(MacroBuildDateHeader)
    BUILD_DATE_HEADER()

    # ==========================================================================
    # To prevent warnings from M$ compiler

    if(WIN32 AND MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-D_ATL_SECURE_NO_WARNINGS)
        add_definitions(-D_AFX_SECURE_NO_WARNINGS)
    endif()

    # ==========================================================================
    # To use ccache with compiler

    if(HAVE_CCACHE)
        message(STATUS "Using ccache to speed-up compilations..... YES")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    ccache)
    else()
        message(STATUS "Using ccache to speed-up compilations..... NO")
    endif()

    # ==========================================================================
    # Definitions rules

    # TODO: revise dropped compiler flags accordingly with patched source code which fix compilation

    # Remove pedantic GCC flag which generate a lots of warnings on the console
    # with qCDebug()
    REMOVE_GCC_COMPILER_WARNINGS("-pedantic")

    # Remove suggest-override GCC flag which generate a lots of compilation warnings
    REMOVE_GCC_COMPILER_WARNINGS("-Wsuggest-override")

    # Remove cast-align GCC flag which generate a lots of compilation warnings
    REMOVE_GCC_COMPILER_WARNINGS("-Wcast-align")

    # Remove zero-as-null-pointer-constant GCC and CLang flag which generate a lots of compilation warnings
    REMOVE_COMPILER_WARNINGS("-Wzero-as-null-pointer-constant")

    # Remove deprecated-copy GCC warnings which generate a lots of compilation warnings
    DISABLE_GCC_COMPILER_WARNINGS("8.99.99" "-Wno-deprecated-copy")

    # Remove inconsistent-missing-override Clang warnings which generate a lots of compilation warnings
    DISABLE_CLANG_COMPILER_WARNINGS("7.99.99" "-Wno-inconsistent-missing-override")

    include(MacroSanitizers)
    ENABLE_COMPILER_SANITIZERS()

    # translations catalog
    add_definitions(-DTRANSLATION_DOMAIN=\"digikam\")

    # NOTE: with libpgf 6.11.24 OpenMP is not very well supported. We disable
    # it to be safe. See B.K.O #273765 for details.
    add_definitions(-DLIBPGF_DISABLE_OPENMP)

    # Enable C++ Exceptions support, require by Greycstoration algorithm
    # (CImg.h) and PGF codec
    kde_enable_exceptions()

    # Enforce modern Qt code
    add_definitions(-DQT_DEPRECATED_WARNINGS
                    -DQT_USE_QSTRINGBUILDER
                    -DQT_NO_CAST_TO_ASCII
                    -DQT_NO_CAST_FROM_ASCII
                    -DQT_NO_CAST_FROM_BYTEARRAY
                    -DQT_NO_URL_CAST_FROM_STRING
                    -DQT_STRICT_ITERATORS
                    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
                    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
#                    -DQT_DISABLE_DEPRECATED_BEFORE=0x050E00
#                    -DQT_NO_FOREACH
#                    -DQT_NO_KEYWORDS
    )

    # ==========================================================================
    # Includes rules

    # Recursively get all directories which contain header files
    set(DK_INCLUDES_ALL "")

    HEADER_DIRECTORIES(DK_LOCAL_INCLUDES_RAW)

    # This macro will set all paths which do not contain libjpeg-
    # We will add later the directory we need

    foreach(var ${DK_LOCAL_INCLUDES_RAW})
        string(REGEX MATCH "libjpeg/" item ${var})
        if(item STREQUAL "")
            list(APPEND DK_LOCAL_INCLUDES ${var})
        endif()
    endforeach()

    set(DK_LOCAL_INCLUDES ${DK_LOCAL_INCLUDES}
                          libs/jpegutils/${DIGIKAM_LIBJPEG_DIR})

    include_directories(${DK_LOCAL_INCLUDES})

    # for config headers digikam_version.h digikam_gitversion.h digikam_config.h
    # digikam_dbconfig.h digikam_opencv.h
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/app/utils)

    include_directories(${OpenCV_INCLUDE_DIRS})

    # for libheif version header
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/heifutils)

    if(ImageMagick_FOUND)
        include_directories(${ImageMagick_INCLUDE_DIRS})
    endif()

    # ==========================================================================
    # Common targets parts

    add_subdirectory(data)
    add_subdirectory(libs)
    add_subdirectory(utilities)
    add_subdirectory(app)
    add_subdirectory(dplugins)
    add_subdirectory(showfoto)

    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()

endif()

# ==============================================================================
# API documentation generation

if(Doxygen_FOUND)

    message(STATUS "Doxygen is found.......................... ${Doxygen_FOUND}")
    message(STATUS "Graphviz Dot is found..................... ${DOXYGEN_DOT_FOUND}")

    if(Doxygen_FOUND AND DOXYGEN_DOT_FOUND)

        message(STATUS "To build API documentation use 'make doc'")

        add_custom_target(doc ${DOXYGEN_EXECUTABLE}
                          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    endif()

endif()
