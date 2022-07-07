#
# Copyright (c) 2010-2022, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()


set(ocrTesseract_cli_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ocrTesseract_cli.cpp
)

add_executable(ocrTesseract_cli ${ocrTesseract_cli_SRCS})

target_link_libraries(ocrTesseract_cli

                      mjpegstreambackend
                      digikamcore

                      ${COMMON_TEST_LINK}
 )
