#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# To fill MacOS and Windows bundles metadata

set(BUNDLE_APP_NAME_STRING          "digikam")
set(BUNDLE_APP_DESCRIPTION_STRING   "Advanced digital photo management application")
set(BUNDLE_LEGAL_COPYRIGHT_STRING   "GNU Public License V2")
set(BUNDLE_COMMENT_STRING           "Free and open source software to manage photo")
set(BUNDLE_LONG_VERSION_STRING      ${DIGIKAM_VERSION_STRING})
set(BUNDLE_SHORT_VERSION_STRING     ${DIGIKAM_VERSION_SHORT})
set(BUNDLE_VERSION_STRING           ${DIGIKAM_VERSION_STRING})

# digiKam executable

set(digikam_SRCS
    main/main.cpp
)

# Set the application icon on the application

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/core/data/icons/apps/*-apps-digikam.png")
ecm_add_app_icon(digikam_SRCS ICONS ${ICONS_SRCS})

if (WIN32)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/versioninfo.rc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)
    add_executable(digikam ${digikam_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

elseif (APPLE)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/DigikamInfo.plist.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    add_executable(digikam ${digikam_SRCS})
    set_target_properties(digikam PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

else()

    add_executable(digikam ${digikam_SRCS})

endif()

add_dependencies(digikam digikam-gitversion)
add_dependencies(digikam digikam-builddate)

target_link_libraries(digikam

                      PUBLIC

                      Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Sql

                      KF5::WindowSystem
                      KF5::I18n
                      KF5::XmlGui
                      KF5::ConfigCore
                      KF5::Service

                      digikamcore
                      digikamdatabase
                      digikamgui
)

if(ENABLE_DBUS)

    target_link_libraries(digikam
                          PUBLIC
                          Qt5::DBus
    )

endif()

if(KF5IconThemes_FOUND)

    target_link_libraries(digikam
                          PUBLIC
                          KF5::IconThemes
    )

endif()

if(KF5KIO_FOUND)

    target_link_libraries(digikam
                          PUBLIC
                          KF5::KIOWidgets
    )

endif()

if(ImageMagick_Magick++_FOUND)

    target_link_libraries(digikam
                          PUBLIC
                          ${ImageMagick_LIBRARIES}
    )

endif()

install(TARGETS digikam ${INSTALL_TARGETS_DEFAULT_ARGS})
