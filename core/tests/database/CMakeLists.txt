#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5Notifications_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::Notifications,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

#------------------------------------------------------------------------

set(testdatabaseswitch_SRCS testdatabaseswitch.cpp)
add_executable(testdatabaseswitch ${testdatabaseswitch_SRCS})

target_link_libraries(testdatabaseswitch
                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}

                      ${OpenCV_LIBRARIES}
)

#------------------------------------------------------------------------

set(testdatabaseinit_SRCS testdatabaseinit.cpp)
add_executable(testdatabaseinit ${testdatabaseinit_SRCS})

target_link_libraries(testdatabaseinit
                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}

                      ${OpenCV_LIBRARIES}
)

#------------------------------------------------------------------------

set(databasefieldstest_src databasefieldstest.cpp)

ecm_add_tests(${databasefieldstest_src}

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


set(databasetagstest_src databasetagstest.cpp)
add_executable(databasetagstest ${databasetagstest_src})

target_link_libraries(databasetagstest

                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}
)

# NOTE: this unit test fail to work as the temporary collection creation cannot be done in /tmp.
#       we need to port code to use QTest directory, as it done with metadataengine unit-tests.
#
#ecm_add_tests(${databasetagstest_src}
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
