#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

add_executable(mjpegstream_cli ${CMAKE_CURRENT_SOURCE_DIR}/mjpegstream_cli.cpp)

target_link_libraries(mjpegstream_cli

                      mjpegstreambackend
                      digikamcore

                      ${COMMON_TEST_LINK}
 )
