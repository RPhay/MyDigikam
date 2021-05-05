#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

##################################################################

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/searchtextbartest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

##################################################################

add_executable(ditemslisttest ${CMAKE_CURRENT_SOURCE_DIR}/ditemslisttest.cpp)

target_link_libraries(ditemslisttest

                      digikamcore

                      ${COMMON_TEST_LINK}
)
