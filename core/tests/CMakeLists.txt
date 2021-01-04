#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# enabling for everything although only required by DImg.
kde_enable_exceptions()

# -------------------------------------------------------------------------------
# Unit-tests common dependencies

if(ENABLE_QWEBENGINE)

    set(_extra_deps ${_extra_deps} Qt5::WebEngineWidgets)

else()

    set(_extra_deps ${_extra_deps} Qt5::WebKitWidgets)

endif()

if(ENABLE_DBUS)

    set(_extra_deps ${_extra_deps} Qt5::DBus)

endif()

if(KF5Notifications_FOUND)

    set(_extra_deps ${_extra_deps} KF5::Notifications)

endif()

if(Gphoto2_FOUND)

    set(_extra_deps ${_extra_deps} ${GPHOTO2_LIBRARIES})

endif()

if(ImageMagick_Magick++_FOUND)

    set(_extra_deps ${_extra_deps} ${ImageMagick_LIBRARIES})

endif()

if(LensFun_FOUND)

    set(_extra_deps ${_extra_deps} ${LENSFUN_LIBRARIES})

endif()

if(Jasper_FOUND)

    set(_extra_deps ${_extra_deps} ${JASPER_LIBRARIES})

endif()

if(ENABLE_MEDIAPLAYER)

    set(_extra_deps ${_extra_deps} ${QTAV_LIBRARIES})

endif()

if(WIN32)

    set(_extra_deps ${_extra_deps} ws2_32)

endif()

if(NOT WIN32)

    # To link under Solaris (see bug #274484)
    set(_extra_deps ${_extra_deps} ${MATH_LIBRARY})

endif()

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)

    set(_extra_deps ${_extra_deps} ${KVM_LIBRARY})

endif()

if(APPLE)

    set(_extra_deps ${_extra_deps} /System/Library/Frameworks/AppKit.framework)

endif()

set(COMMON_TEST_LINK Qt5::Core
                     Qt5::Gui
                     Qt5::Xml
                     Qt5::Widgets
                     Qt5::Concurrent
                     Qt5::Test
                     Qt5::Sql

                     KF5::XmlGui
                     KF5::I18n
                     KF5::ConfigCore
                     KF5::Service
                     KF5::Solid

                     ${OPENMP_LDFLAGS}
                     ${JPEG_LIBRARIES}
                     ${LCMS2_LIBRARIES}
                     ${EXPAT_LIBRARY}
                     ${CMAKE_THREAD_LIBS_INIT}

                     opencv_core
                     opencv_objdetect
                     opencv_imgproc
                     opencv_imgcodecs
                     opencv_dnn
                     opencv_flann

                     ${_extra_deps}
)

# -------------------------------------------------------------------------------
# Common static library for uint-tests based on ModelTest

include_directories(modeltest)

add_library(libmodeltest STATIC
            modeltest/modeltest.cpp
)

target_link_libraries(libmodeltest
                      PRIVATE
                      Qt5::Core
                      Qt5::Test
)

# -------------------------------------------------------------------------------
# Unit-tests and CLI tools

add_subdirectory(advancedrename)
add_subdirectory(albummodel)                    # Unit-test based on ModelTest
add_subdirectory(database)                      # Some unit-tests based on ModelTest
add_subdirectory(dimg)
add_subdirectory(dngwriter)
add_subdirectory(dplugins)
add_subdirectory(facesengine)
add_subdirectory(fileio)
add_subdirectory(filters)
add_subdirectory(imgqsort)
add_subdirectory(iojobs)
add_subdirectory(mediaserver)
add_subdirectory(mediawiki)
add_subdirectory(metadataengine)
add_subdirectory(miscs)
add_subdirectory(multithreading)
add_subdirectory(rawengine)
add_subdirectory(timestampupdate)
add_subdirectory(webservices)
add_subdirectory(widgets)

if(Marble_FOUND)
    add_subdirectory(geolocation)               # Some unit-tests based on ModelTest
endif()

if(FLEX_FOUND AND BISON_FOUND AND KF5ThreadWeaver_FOUND)
    add_subdirectory(panorama)
endif()

if(ENABLE_MEDIAPLAYER)
    add_subdirectory(video)
endif()

if(KF5KIO_FOUND)
    add_subdirectory(vkontakte)
endif()
