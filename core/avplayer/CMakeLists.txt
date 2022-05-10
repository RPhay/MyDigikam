#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# To fill MacOS and Windows bundles metadata

set(BUNDLE_APP_NAME_STRING          "avplayer")
set(BUNDLE_APP_DESCRIPTION_STRING   "Stand alone Audio and Video Player from digiKam Project")
set(BUNDLE_LEGAL_COPYRIGHT_STRING   "GNU Public License V2")
set(BUNDLE_COMMENT_STRING           "Free and open source software to play audio and video")
set(BUNDLE_LONG_VERSION_STRING      ${DIGIKAM_VERSION_STRING})
set(BUNDLE_SHORT_VERSION_STRING     ${DIGIKAM_VERSION_SHORT})
set(BUNDLE_VERSION_STRING           ${DIGIKAM_VERSION_STRING})

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

if(HAVE_OPENGL)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

endif()

set(avplayer_SRCS ${CMAKE_SOURCE_DIR}/core/avplayer/utils/ClickableMenu.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/EventFilter.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/Slider.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/StatisticsView.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/ScreenSaver.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/qoptions.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/utils/common.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/VideoEQConfigPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/PropertyEditor.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/DecoderConfigPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/AVFormatConfigPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/ShaderPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/MiscPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/ConfigManager.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/ConfigDialog.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/ConfigPageBase.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/CaptureConfigPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/config/AVFilterConfigPage.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/OSDFilter.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/OSD.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/filters/AVFilterSubtitle.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayListModel.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayListItem.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayListDelegate.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/playlist/PlayList.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/main.cpp
                  ${CMAKE_SOURCE_DIR}/core/avplayer/MainWindow.cpp
)

if(Qt6_FOUND)

    qt6_add_resources(avplayer_SRCS ${CMAKE_SOURCE_DIR}/core/avplayer/theme/theme.qrc)

else()

    qt5_add_resources(avplayer_SRCS ${CMAKE_SOURCE_DIR}/core/avplayer/theme/theme.qrc)

endif()



# Set the application icon on the application

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/core/data/icons/apps/*-apps-avplayer.png")
ecm_add_app_icon(avplayer_SRCS ICONS ${ICONS_SRCS})

if (WIN32)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/versioninfo.rc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)
    add_executable(avplayer ${showfoto_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

elseif (APPLE)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/AVPlayerInfo.plist.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    add_executable(avplayer ${avplayer_SRCS})
    set_target_properties(avplayer PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

else()

    add_executable(avplayer ${avplayer_SRCS})

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

if (APPLE)

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

install(TARGETS  avplayer                                                 ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.avplayer.desktop     DESTINATION ${KDE_INSTALL_FULL_APPDIR})
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.avplayer.appdata.xml DESTINATION ${KDE_INSTALL_FULL_METAINFODIR})
