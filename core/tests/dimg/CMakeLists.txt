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
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(testdimgloader_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/testdimgloader.cpp)
add_executable(testdimgloader ${testdimgloader_SRCS})
ecm_mark_nongui_executable(testdimgloader)

target_link_libraries(testdimgloader

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(testequalizefilter_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/testequalizefilter.cpp)
add_executable(testequalizefilter ${testequalizefilter_SRCS})
ecm_mark_nongui_executable(testequalizefilter)

target_link_libraries(testequalizefilter

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

set(testcolorbalancefilter_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/testcolorbalancefilter.cpp)
add_executable(testcolorbalancefilter ${testcolorbalancefilter_SRCS})
ecm_mark_nongui_executable(testcolorbalancefilter)

target_link_libraries(testcolorbalancefilter

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

if(ImageMagick_Magick++_FOUND)

    set(magickloader_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/magickloader.cpp)
    add_executable(magickloader ${magickloader_SRCS})

    target_link_libraries(magickloader

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

endif()

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimgfilteractiontest.cpp

              GUI

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimgfreerotationtest.cpp

              GUI

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

add_library(libabstracthistory STATIC ${CMAKE_CURRENT_SOURCE_DIR}/dimgabstracthistorytest.cpp)

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimghistorytest.cpp

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

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/dimghistorygraphtest.cpp

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
