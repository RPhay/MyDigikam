#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(testautocrop_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/testautocrop.cpp)
add_executable(testautocrop ${testautocrop_SRCS})
ecm_mark_nongui_executable(testautocrop)

target_link_libraries(testautocrop

                      digikamcore

                      ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

if(LensFun_FOUND)

    include_directories(${LENSFUN_INCLUDE_DIRS})

    set(testlensfuniface_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/testlensfuniface.cpp)
    add_executable(testlensfuniface ${testlensfuniface_SRCS})
    ecm_mark_nongui_executable(testlensfuniface)

    target_link_libraries(testlensfuniface

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

endif()

#------------------------------------------------------------------------

set(testnrestimate_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/testnrestimate.cpp)
add_executable (testnrestimate ${testnrestimate_SRCS})
ecm_mark_nongui_executable(testnrestimate)

target_link_libraries(testnrestimate

                      digikamcore
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
