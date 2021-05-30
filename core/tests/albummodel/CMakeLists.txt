#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

add_executable(albummodel_utest ${CMAKE_CURRENT_SOURCE_DIR}/albummodel_utest.cpp)

target_link_libraries(albummodel_utest

                      digikamcore
                      digikamdatabase
                      digikamgui
                      libmodeltest

                      ${COMMON_TEST_LINK}
)

# TODO: this unit test needs to be re-written, since it depends on database,
# threading etc, therefore it is not predictable and very hard to fix.
# Only a command line version is provided for the moment.
#
#ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/albummodel_utest.cpp
#
#              NAME_PREFIX
#
#              "digikam-"
#
#              LINK_LIBRARIES
#
#              digikamcore
#              digikamdatabase
#              digikamgui
#
#              libmodeltest
#
#              ${COMMON_TEST_LINK}
#)
