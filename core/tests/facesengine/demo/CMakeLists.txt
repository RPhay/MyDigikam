#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(facesenginedemo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/demofaceengine.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demomainwindow.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demofaceitem.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demobutton.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demomarquee.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/demofancyrect.cpp
)

qt5_wrap_ui(facesenginedemo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/demomainwindow.ui)
add_executable(facesenginedemo ${facesenginedemo_SRCS})

target_link_libraries(facesenginedemo

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
