#
# Copyright (c) 2015,      Mohamed_Anwer, <m_dot_anwer at gmx dot com>
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
)

add_executable(IOJobsTest ${CMAKE_CURRENT_SOURCE_DIR}/iojobs_utest.cpp)

ecm_mark_nongui_executable(IOJobsTest)

target_link_libraries(IOJobsTest

                      digikamgui

                      ${COMMON_TEST_LINK}
)
