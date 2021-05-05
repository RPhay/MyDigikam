#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# We need to include the setup dir for showfoto BEFORE the other include dirs,
# otherwise the one from the "utilities" dir will be used
include_directories(BEFORE
                    $<TARGET_PROPERTY:Qt5::Network,INTERFACE_INCLUDE_DIRECTORIES>

                    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar
                    ${CMAKE_CURRENT_SOURCE_DIR}/setup
                    ${CMAKE_CURRENT_SOURCE_DIR}/main
)

set(showfoto_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/setup/showfotosetuptooltip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/setup/showfotosetupmisc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/setup/showfotosetupmetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/setup/showfotosetupraw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/setup/showfotosetupplugins.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/setup/showfotosetup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotoiteminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotoimagemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotothumbnailmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotofiltermodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotodragdrophandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotokineticscroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotoitemsortsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotocoordinatesoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/itemviewshowfotodelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotodelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfototooltipfiller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotocategorizedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbbar/showfotothumbnailbar.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/main/showfotosettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/showfoto.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/showfoto_setup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/showfoto_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/showfoto_import.cpp
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/icons/apps/*-apps-showfoto.png")
ecm_add_app_icon(showfoto_SRCS ICONS ${ICONS_SRCS})

add_executable(showfoto ${showfoto_SRCS})

add_dependencies(showfoto digikam-gitversion)
add_dependencies(showfoto digikam-builddate)

# To fill plist XML file for OSX #####################################################

set(MACOSX_APP_NAME_STRING             "showfoto")
set(MACOSX_APP_DESCRIPTION             "Stand alone Photo Editor based on digiKam Image Editor")
set(MACOSX_BUNDLE_LONG_VERSION_STRING  ${DIGIKAM_VERSION_STRING})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${DIGIKAM_VERSION_SHORT})
set(MACOSX_BUNDLE_BUNDLE_VERSION       ${DIGIKAM_VERSION_STRING})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/Info.plist.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
set_target_properties(showfoto PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

target_link_libraries(showfoto

                      PUBLIC

                      Qt5::Core
                      Qt5::Gui
                      Qt5::Xml
                      Qt5::Widgets

                      KF5::XmlGui
                      KF5::I18n
                      KF5::ConfigCore
                      KF5::Service

                      digikamcore
)

if(KF5Notifications_FOUND)

    target_link_libraries(showfoto
                          PUBLIC
                          KF5::Notifications
    )

endif()

if(KF5NotifyConfig_FOUND)

    target_link_libraries(showfoto
                          PUBLIC
                          KF5::NotifyConfig
    )

endif()

if(ImageMagick_Magick++_FOUND)

    target_link_libraries(showfoto
                          PUBLIC
                          ${ImageMagick_LIBRARIES}
    )

endif()

if(NOT WIN32)

    # To link under Solaris (see bug #274484)
    target_link_libraries(showfoto
                          PUBLIC
                          ${MATH_LIBRARY}
    )

endif()

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)

    target_link_libraries(showfoto
                          PUBLIC
                          ${KVM_LIBRARY}
    )

endif()

if(APPLE)

    target_link_libraries(showfoto
                          PUBLIC
                          /System/Library/Frameworks/AppKit.framework
    )

endif()

install(TARGETS  showfoto                                                      ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.showfoto.desktop     DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.showfoto.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/main/showfotoui5.rc               DESTINATION ${KXMLGUI_INSTALL_DIR}/showfoto)
