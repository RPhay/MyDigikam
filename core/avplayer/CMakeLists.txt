#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

# To fill MacOS and Windows bundles metadata

set(BUNDLE_APP_NAME_STRING          "avplayer")
set(BUNDLE_APP_DESCRIPTION_STRING   "Stand alone Audio and Video Player from digiKam Project")
set(BUNDLE_LEGAL_COPYRIGHT_STRING   "GNU Public License V2")
set(BUNDLE_COMMENT_STRING           "Free and open source software to play audio and video")
set(BUNDLE_LONG_VERSION_STRING      ${DIGIKAM_VERSION_STRING})
set(BUNDLE_SHORT_VERSION_STRING     ${DIGIKAM_VERSION_SHORT})
set(BUNDLE_VERSION_STRING           ${DIGIKAM_VERSION_STRING})

set(avplayer_SRCS ${CMAKE_SOURCE_DIR}/core/avplayer/utils/ConfigDialog.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/StatisticsView.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/ScreenSaver.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/qoptions.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/common.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/EventFilter.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/OSDFilter.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/OSD.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/AVFilterSubtitle.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayListModel.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayListItem.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayListDelegate.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayList.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/main/main.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/main/MainWindow.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/main/MainWindow_p.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/main/MainWindow_setup.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/main/MainWindow_io.cpp
)

# Set the application icon on the application

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/core/data/icons/apps/*-apps-avplayer.png")

if(WIN32)

    # Build the main implementation into a DLL to be called by a stub EXE.
    # This is a work around "command line is too long" issue on Windows.
    # see https://stackoverflow.com/questions/43184251/cmake-command-line-too-long-windows

    add_library(avplayer SHARED ${avplayer_SRCS})
    set_target_properties(avplayer PROPERTIES PREFIX "")

elseif(APPLE)

    ecm_add_app_icon(avplayer_SRCS ICONS ${ICONS_SRCS})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/AVPlayerInfo.plist.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    add_executable(avplayer ${avplayer_SRCS})
    set_target_properties(avplayer PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

else()

    ecm_add_app_icon(avplayer_SRCS ICONS ${ICONS_SRCS})
    add_executable(avplayer ${avplayer_SRCS})

endif()

target_include_directories(avplayer
                           PRIVATE

                           $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                           $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                           $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                           $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>

                           $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                           $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                           $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>

                           ${CMAKE_CURRENT_SOURCE_DIR}/filters
                           ${CMAKE_CURRENT_SOURCE_DIR}/playlist
                           ${CMAKE_CURRENT_SOURCE_DIR}/utils
                           ${CMAKE_CURRENT_SOURCE_DIR}/main

                           ${DK_LOCAL_INCLUDES}
)

if(HAVE_OPENGL)

    target_include_directories(avplayer
                               PRIVATE
                               $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>
    )

    if(Qt6_FOUND)

        target_include_directories(avplayer
                                   PRIVATE
                                   $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>
        )

    endif()

endif()

add_dependencies(avplayer digikam-gitversion)
add_dependencies(avplayer digikam-builddate)

target_link_libraries(avplayer
                      digikamcore

                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::Sql

                      KF5::XmlGui
                      KF5::I18n
                      KF5::ConfigCore
                      KF5::Service

                      ${MEDIAPLAYER_LIBRARIES}
)

if(APPLE)

    target_link_libraries(avplayer
                          "-framework CoreServices"
                          "-framework ScreenSaver"
                          "-framework IOKit"
    )

endif()

target_compile_definitions(avplayer
                           PRIVATE
                           ${MEDIAPLAYER_DEFINITIONS}
)

install(TARGETS  avplayer                                                      ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.avplayer.desktop     DESTINATION ${KDE_INSTALL_FULL_APPDIR})
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.avplayer.appdata.xml DESTINATION ${KDE_INSTALL_FULL_METAINFODIR})

if(WIN32)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/versioninfo.rc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

    set(avplayer_windows_stub_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/main/windows_stub_main.cpp)

    ecm_add_app_icon(avplayer_windows_stub_SRCS ICONS ${ICONS_SRCS})

    add_executable(avplayer_windows_stub_exe
                   ${avplayer_windows_stub_SRCS}
                   ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc
    )

    target_link_libraries(avplayer_windows_stub_exe PRIVATE avplayer Qt5::WinMain)
    set_target_properties(avplayer_windows_stub_exe PROPERTIES OUTPUT_NAME "avplayer")
    target_include_directories(avplayer_windows_stub_exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main)

    install(TARGETS avplayer_windows_stub_exe ${INSTALL_TARGETS_DEFAULT_ARGS})

endif()
