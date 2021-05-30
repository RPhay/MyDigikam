#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(preprocess_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/preprocess_gui.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/tantriggspreprocessor.cpp
)

add_executable(preprocess_gui ${preprocess_gui_SRCS})

target_link_libraries(preprocess_gui

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
