#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

cmake_minimum_required(VERSION 3.3.2)

if(POLICY CMP0063)
    # C++ symbols visibility policy introduced in CMake version 3.3
    # Details: https://cmake.org/cmake/help/git-stage/policy/CMP0063.html
    cmake_policy(SET CMP0063 NEW)
endif()

find_package(Qt5 NO_MODULE REQUIRED Core)
get_target_property(QT_RCC_EXECUTABLE Qt5::rcc LOCATION)

add_executable(qrcAlias main.cpp)
target_link_libraries(qrcAlias PUBLIC Qt5::Core)

add_custom_command(OUTPUT breeze.rcc
    COMMAND ${QT_RCC_EXECUTABLE} -project -o icons.qrc
    COMMAND $<TARGET_FILE_DIR:qrcAlias>/$<TARGET_FILE_NAME:qrcAlias> -i icons.qrc -o aliased.qrc
    COMMAND ${QT_RCC_EXECUTABLE} -binary -o ${CMAKE_BINARY_DIR}/breeze.rcc -root /icons/breeze aliased.qrc

    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/icons/breeze
    DEPENDS qrcAlias
)

add_custom_command(OUTPUT breeze-dark.rcc
    COMMAND ${QT_RCC_EXECUTABLE} -project -o icons.qrc
    COMMAND $<TARGET_FILE_DIR:qrcAlias>/$<TARGET_FILE_NAME:qrcAlias> -i icons.qrc -o aliased.qrc
    COMMAND ${QT_RCC_EXECUTABLE} -binary -o ${CMAKE_BINARY_DIR}/breeze-dark.rcc -root /icons/breeze-dark aliased.qrc

    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/icons/breeze-dark
    DEPENDS qrcAlias
)

add_custom_target(icons ALL
    DEPENDS breeze.rcc breeze-dark.rcc
)

install(FILES ${CMAKE_BINARY_DIR}/breeze.rcc      DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
install(FILES ${CMAKE_BINARY_DIR}/breeze-dark.rcc DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
