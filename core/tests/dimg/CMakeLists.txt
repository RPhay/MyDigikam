#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# Boost uses operator names (and, not, ...)
string(REPLACE "-fno-operator-names" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(dimgloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/dimgloader_cli.cpp)
add_executable(dimgloader_cli ${dimgloader_cli_SRCS})
ecm_mark_nongui_executable(dimgloader_cli)

target_link_libraries(dimgloader_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(equalizefilter_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/equalizefilter_cli.cpp)
add_executable(equalizefilter_cli ${equalizefilter_cli_SRCS})
ecm_mark_nongui_executable(equalizefilter_cli)

target_link_libraries(equalizefilter_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(colorbalancefilter_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/colorbalancefilter_cli.cpp)
add_executable(colorbalancefilter_cli ${colorbalancefilter_cli_SRCS})
ecm_mark_nongui_executable(colorbalancefilter_cli)

target_link_libraries(colorbalancefilter_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

if(ImageMagick_Magick++_FOUND)

    set(magickloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/magickloader_cli.cpp)
    add_executable(magickloader_cli ${magickloader_cli_SRCS})

    target_link_libraries(magickloader_cli

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

endif()

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimgfilteraction_utest.cpp

              GUI

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimgfreerotation_utest.cpp

              GUI

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

add_library(libabstracthistory STATIC ${CMAKE_CURRENT_SOURCE_DIR}/dimgabstracthistory_utest.cpp)

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimghistory_utest.cpp

              GUI

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              libabstracthistory

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

# TODO: This unit-test do not link yet under Microsoft compiler about HistoryGraphData

if(NOT MSVC)

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimghistorygraph_utest.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore
                  digikamdatabase
                  libmodeltest
                  libabstracthistory

                  ${COMMON_TEST_LINK}
    )

endif()
