#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/geoparsing_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

add_test(geoparsing_utest ${EXECUTABLE_OUTPUT_PATH}/geoparsing_utest)
