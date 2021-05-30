#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(autocrop_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/autocrop_cli.cpp)
add_executable(autocrop_cli ${autocrop_cli_SRCS})
ecm_mark_nongui_executable(autocrop_cli)

target_link_libraries(autocrop_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

if(LensFun_FOUND)

    include_directories(${LENSFUN_INCLUDE_DIRS})

    set(lensfuniface_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/lensfuniface_cli.cpp)
    add_executable(lensfuniface_cli ${lensfuniface_cli_SRCS})
    ecm_mark_nongui_executable(lensfuniface_cli)

    target_link_libraries(lensfuniface_cli

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

endif()

#------------------------------------------------------------------------

set(nrestimate_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/nrestimate_cli.cpp)
add_executable(nrestimate_cli ${nrestimate_cli_SRCS})
ecm_mark_nongui_executable(nrestimate_cli)

target_link_libraries(nrestimate_cli

                      digikamcore
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
