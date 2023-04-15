#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5Notifications_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::Notifications,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

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

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/databasesqliteinit_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

if (ENABLE_MYSQLSUPPORT AND ENABLE_INTERNALMYSQL)

# TODO: do not work yet.
    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/databaseswitch_utest.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore
                  digikamdatabase
                  digikamgui

                  ${COMMON_TEST_LINK}
    )

    # ---

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/databasemysqlinit_utest.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore
                  digikamdatabase
                  digikamgui

                  ${COMMON_TEST_LINK}
    )

endif()

#------------------------------------------------------------------------

add_executable(databasetags_utest ${CMAKE_CURRENT_SOURCE_DIR}/databasetags_utest.cpp)

target_link_libraries(databasetags_utest

                      digikamcore
                      digikamdatabase
                      digikamgui

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

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

#------------------------------------------------------------------------

add_executable(tagscache_utest ${CMAKE_CURRENT_SOURCE_DIR}/tagscache_utest.cpp)

target_link_libraries(tagscache_utest
                      digikamcore
                      digikamdatabase
                      ${COMMON_TEST_LINK}
)

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/haariface_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              ${COMMON_TEST_LINK}

              GUI
)
