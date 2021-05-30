#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(pgfscaled_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/pgfscaled_cli.cpp ${pgfutils_SRCS})
add_executable(pgfscaled_cli ${pgfscaled_cli_SRCS})
ecm_mark_nongui_executable(pgfscaled_cli)

target_link_libraries(pgfscaled_cli
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(qtpgftest_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/qtpgftest_cli.cpp ${pgfutils_SRCS})
add_executable(qtpgftest_cli ${qtpgftest_cli_SRCS})
ecm_mark_nongui_executable(qtpgftest_cli)

target_link_libraries(qtpgftest_cli
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(loadpgfdata_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/loadpgfdata_cli.cpp ${pgfutils_SRCS})
add_executable(loadpgfdata_cli ${loadpgfdata_cli_SRCS})
ecm_mark_nongui_executable(loadpgfdata_cli)

target_link_libraries(loadpgfdata_cli
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(loadsavethread_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/loadsavethread_cli.cpp)
add_executable(loadsavethread_cli ${loadsavethread_cli_SRCS})
ecm_mark_nongui_executable(loadsavethread_cli)

target_link_libraries(loadsavethread_cli
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/filesaveoptionsbox_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/statesavingobject_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)
