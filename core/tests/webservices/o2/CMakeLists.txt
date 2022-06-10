#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

DISABLE_CLANG_COMPILER_WARNINGS("4.99.99" "-Wno-extra-semi")

# The o2 library does not adhere to the flags we use
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050E00
    -DQT_NO_KEYWORDS
)

include_directories(${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/src)

if(NOT APPLE)

    add_subdirectory(fbdemo)

    if(ENABLE_QWEBENGINE)
# NOTE: Do not compile anymore on CI
#        add_subdirectory(fbinter)
    endif()

endif()

if(Qt5Qml_FOUND AND Qt5WebView_FOUND)
    add_subdirectory(sialis)
endif()

add_subdirectory(twdemo)
