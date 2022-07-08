#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(BUILD_WITH_QT6)

    find_package(Qt6 ${QT6_MIN_VERSION} REQUIRED COMPONENTS Core)

    set(QT_VERSION       ${Qt6Core_VERSION})
    set(QT_MIN_VERSION   ${QT6_MIN_VERSION})
    set(QT_VERSION_MAJOR 6)

else()

    find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED COMPONENTS Core)

    set(QT_VERSION       ${Qt5Core_VERSION})
    set(QT_MIN_VERSION   ${QT5_MIN_VERSION})
    set(QT_VERSION_MAJOR 5)

endif()

message(STATUS "Suitable Qt${QT_VERSION_MAJOR} >= ${QT_MIN_VERSION} detected: '${QT_VERSION}'.")
