#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

function(generate_binary_resource target outfile)
    set(RESOURCES_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/res)
    set(RESOURCE_FILE ${RESOURCES_WORKING_DIR}/breeze-${target}.qrc)
    set(BINARY_RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/breeze-${target}.rcc)

    # Use $<IF:$<BOOL:${MSVC}>,PATH,LD_LIBRARY_PATH> instead of ${pathVarName} once CMake 3.8 is minimum
    if(MSVC)
        set(pathVarName PATH)
    else()
        set(pathVarName LD_LIBRARY_PATH)
    endif()

    get_target_property(QT_RCC_EXECUTABLE Qt5::rcc LOCATION)

    add_custom_target(breeze-${target}-mkdir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RESOURCES_WORKING_DIR}
    )
    add_custom_command(OUTPUT ${RESOURCE_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${RESOURCES_WORKING_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove
            ${RESOURCE_FILE}
            ${RESOURCE_FILE}.depends
            ${RESOURCES_WORKING_DIR}/.gitignore
            ${RESOURCES_WORKING_DIR}/CMakeLists.txt
        COMMAND ${QT_RCC_EXECUTABLE} --project -o ${CMAKE_CURRENT_BINARY_DIR}/tmp.qrc
        COMMAND ${CMAKE_COMMAND} -E env
            ${pathVarName}=$<TARGET_FILE_DIR:Qt5::Core>
            $<TARGET_FILE:qrcAlias> -i ${CMAKE_CURRENT_BINARY_DIR}/tmp.qrc -o ${RESOURCE_FILE}

        WORKING_DIRECTORY ${RESOURCES_WORKING_DIR}
        DEPENDS breeze-${target}-mkdir
    )
    add_custom_command(OUTPUT ${BINARY_RESOURCE_FILE}
        COMMAND ${QT_RCC_EXECUTABLE} --binary
                    -o ${BINARY_RESOURCE_FILE}
                    ${RESOURCE_FILE}
        DEPENDS ${RESOURCE_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_custom_target(breeze-${target}-rcc ALL DEPENDS ${BINARY_RESOURCE_FILE})

    set(${outfile} ${BINARY_RESOURCE_FILE} PARENT_SCOPE)
endfunction()

add_executable(qrcAlias qrcAlias.cpp)
target_link_libraries(qrcAlias PUBLIC Qt5::Core)

generate_binary_resource(icons binary_resource)
generate_binary_resource(icons-dark binary_resource)

install(FILES ${CMAKE_BINARY_DIR}/breeze.rcc      DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
install(FILES ${CMAKE_BINARY_DIR}/breeze-dark.rcc DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

