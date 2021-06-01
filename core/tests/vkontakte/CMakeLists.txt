#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(ECMAddTests)

add_definitions(-DAUTOTESTS_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DAUTOTESTS_API_TOKEN_PATH="${CMAKE_CURRENT_SOURCE_DIR}/api-token.txt")

add_library(testhelper STATIC vktestbase.cpp)
target_link_libraries(testhelper Qt5::Core Qt5::Test)

function(add_vk_test name)

    ecm_add_test(${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp
                 LINK_LIBRARIES testhelper
                                digikamcore
                                vkontaktebackend
                                ${COMMON_TEST_LINK}
                                KF5::KIOCore
    )

endfunction()

add_vk_test(userinfo_utest)
add_vk_test(albums_utest)
add_vk_test(photos_utest)
add_vk_test(frequentrequests_utest)
