#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5Notifications_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::Notifications,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

#------------------------------------------------------------------------

add_executable(checkdatabaseswitch_cli ${CMAKE_CURRENT_SOURCE_DIR}/checkdatabaseswitch_cli.cpp)

target_link_libraries(checkdatabaseswitch_cli

                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

add_executable(checkdatabaseinit_cli ${CMAKE_CURRENT_SOURCE_DIR}/checkdatabaseinit_cli.cpp)

target_link_libraries(checkdatabaseinit_cli

                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/databasefields_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              libmodeltest

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

add_executable(databasetags_utest ${CMAKE_CURRENT_SOURCE_DIR}/databasetags_utest.cpp)

target_link_libraries(databasetags_utest

                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}
)

# NOTE: this unit test fail to work as the temporary collection creation cannot be done in /tmp.
#       we need to port code to use QTest directory, as it done with metadataengine unit-tests.
#
#ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/databasetags_utest.cpp
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
