#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

add_custom_command(OUTPUT face-funnel.data
                   COMMAND ${CMAKE_COMMAND} -E tar xz ${CMAKE_CURRENT_SOURCE_DIR}/face-funnel.data.tgz)

add_custom_target(extract_funnel_data ALL DEPENDS face-funnel.data)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(align_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/align.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/funnelreal.cpp
)

add_executable(align ${align_SRCS})
add_dependencies(align extract_funnel_data)

target_link_libraries(align

                      digikamcore
                      digikamgui
                      digikamfacesengine
                      digikamdatabase

                      ${COMMON_TEST_LINK}
)
