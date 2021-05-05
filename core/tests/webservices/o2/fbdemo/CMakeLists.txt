#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# =======================================================
# Facebook demo test tool

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
)

# The o2 library does not adhere to the flags we use
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY)

set(fbdemo_SRCS ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookdemo/fbdemo.cpp
                ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/examples/facebookdemo/main.cpp
)

add_executable(fbdemo ${fbdemo_SRCS})

target_link_libraries(fbdemo
                      digikamcore

                      ${COMMON_TEST_LINK}
)

target_compile_definitions(fbdemo PRIVATE signals=Q_SIGNALS slots=Q_SLOTS)

if(MSVC)

    target_compile_options(fbdemo PRIVATE /w)

else()

    target_compile_options(fbdemo PRIVATE -w)

endif()
