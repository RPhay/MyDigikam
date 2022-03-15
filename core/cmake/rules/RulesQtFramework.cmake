#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
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

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED
                                       NO_MODULE
                                       COMPONENTS
                                       WebEngineWidgets
    )

else()

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED
                                       NO_MODULE
                                       COMPONENTS
                                       WebKitWidgets
    )

endif()

find_package(Qt${QT_VERSION_MAJOR}
             OPTIONAL_COMPONENTS
             DBus
             OpenGL
)

if(Qt6_FOUND)

    find_package(Qt${QT_VERSION_MAJOR}
                 OPTIONAL_COMPONENTS
                 OpenGLWidgets
    )

else()

    find_package(Qt${QT_VERSION_MAJOR}
                 OPTIONAL_COMPONENTS
                 XmlPatterns                # For Rajce plugin
    )

endif()

if(ENABLE_DBUS)

    if(NOT Qt${QT_VERSION_MAJOR}DBus_FOUND)

        set(ENABLE_DBUS OFF)

    endif()

endif()

# Qt Dependencies For unit tests and CLI test tools

if(BUILD_TESTING)

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED
                                       NO_MODULE
                                       COMPONENTS
                                       Test
    )

    find_package(Qt${QT_VERSION_MAJOR}
                 QUIET
                 OPTIONAL_COMPONENTS
                 Qml WebView  # Optional, for 'sialis' O2 library test tool.
    )

endif()
