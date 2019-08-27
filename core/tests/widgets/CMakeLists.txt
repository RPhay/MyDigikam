#
# Copyright (c) 2010-2019 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(searchtextbartest_SRCS
    searchtextbartest.cpp
)

add_executable(searchtextbartest ${searchtextbartest_SRCS})
add_test(searchtextbartest searchtextbartest)
ecm_mark_as_test(searchtextbartest)

target_link_libraries(searchtextbartest
                      digikamcore

                      ${COMMON_TEST_LINK}
)

##################################################################

set(ditemslisttest_SRCS
    ditemslisttest.cpp
)

add_executable(ditemslisttest ${ditemslisttest_SRCS})

target_link_libraries(ditemslisttest
                      digikamcore

                      ${COMMON_TEST_LINK}
)
