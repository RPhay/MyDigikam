#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>
)


# The o2 library does not adhere to the flags we use
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY)

# =======================================================
# Facebook demo test tool

set(fbinter_SRCS ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookexternalinterceptordemo/fbdemo.cpp
                 ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookexternalinterceptordemo/main.cpp
                 ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookexternalinterceptordemo/webenginepage.cpp
                 ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookexternalinterceptordemo/webwindow.cpp
)

qt5_wrap_ui(fbinter_SRCS
            ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookexternalinterceptordemo/webwindow.ui
)

add_executable(fbinter ${fbinter_SRCS})

target_link_libraries(fbinter
                      digikamcore

                      ${COMMON_TEST_LINK}
                      Qt5::WebEngineWidgets
)

target_compile_definitions(fbinter PRIVATE signals=Q_SIGNALS slots=Q_SLOTS)

if(MSVC)

    target_compile_options(fbinter PRIVATE /w)

else()

    target_compile_options(fbinter PRIVATE -w)

endif()
