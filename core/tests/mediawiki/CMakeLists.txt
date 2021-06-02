#
# Copyright (c) 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

add_subdirectory(examples)

add_library(libfakeserver STATIC ${CMAKE_CURRENT_SOURCE_DIR}/fakeserver/fakeserver.cpp)

target_link_libraries(libfakeserver ${COMMON_TEST_LINK})

# ----------------------------------------------------------------------------------

macro(mediawiki_executable_tests)

    foreach(_testname ${ARGN})

        ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.cpp

                      NAME_PREFIX

                      "digikam-"

                      LINK_LIBRARIES

                      digikamcore
                      mediawikibackend
                      libfakeserver

                      ${COMMON_TEST_LINK}
        )

    endforeach()

endmacro()

mediawiki_executable_tests(
    edit_utest
    #login_utest
    logout_utest
    mediawiki_utest
    parse_utest
    #queryimageinfo_utest
    queryimages_utest
    queryinfo_utest
    queryrevision_utest
    querysiteinfogeneral_utest
    querysiteinfousergroups_utest
    upload_utest
)

# ----------------------------------------------------------------------------------

macro(MEDIAWIKI_TEST_RESOURCES)

    foreach(resource ${ARGN})

        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${resource}"
                           COMMAND ${CMAKE_COMMAND} -E copy
                           "${CMAKE_CURRENT_SOURCE_DIR}/data/${resource}"
                           "${CMAKE_CURRENT_BINARY_DIR}/${resource}"
                           DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/${resource}")

        list(APPEND test_resources_dest "${resource}")

    endforeach()

endmacro()

MEDIAWIKI_TEST_RESOURCES(
    parsetest.rc
    queryrevisiontest.rc
    queryrevisiontest_content.rc
    queryrevisiontest_cuted.rc
    queryrevisiontest_onetitle.rc
    queryrevisiontest_parsetree.rc
    queryrevisiontest_rollback.rc
    queryrevisiontest_timestamponly.rc
    queryrevisiontest_useronly.rc
    uploadtest_image.jpg
)

add_custom_target(TestResources ALL DEPENDS ${test_resources_dest})
