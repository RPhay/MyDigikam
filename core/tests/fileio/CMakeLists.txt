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

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(pgfscaled_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/pgfscaled.cpp ${pgfutils_SRCS})
add_executable(pgfscaled ${pgfscaled_SRCS})
ecm_mark_nongui_executable(pgfscaled)

target_link_libraries(pgfscaled
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(qtpgftest_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/qtpgftest.cpp ${pgfutils_SRCS})
add_executable(qtpgftest ${qtpgftest_SRCS})
ecm_mark_nongui_executable(qtpgftest)

target_link_libraries(qtpgftest
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(loadpgfdata_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/loadpgfdata.cpp ${pgfutils_SRCS})
add_executable(loadpgfdata ${loadpgfdata_SRCS})
ecm_mark_nongui_executable(loadpgfdata)

target_link_libraries(loadpgfdata
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(loadsavethreadtest_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/loadsavethreadtest.cpp)
add_executable(loadsavethreadtest ${loadsavethreadtest_SRCS})
ecm_mark_nongui_executable(loadsavethreadtest)

target_link_libraries(loadsavethreadtest
                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/filesaveoptionsboxtest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/statesavingobjecttest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)
