#
# Copyright (c) 2015,      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2010-2020, by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

set(dmetadataloader_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/dmetadataloader_cli.cpp)
add_executable(dmetadataloader_cli ${dmetadataloader_cli_SRCS})
ecm_mark_nongui_executable(dmetadataloader_cli)

target_link_libraries(dmetadataloader_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

set(exportchanges_cli_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/exportchanges_cli.cpp)
add_executable(exportchanges_cli ${exportchanges_cli_SRCS})
ecm_mark_nongui_executable(exportchanges_cli)

target_link_libraries(exportchanges_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# ----------------------------------------------------------------------------------

QT5_WRAP_CPP(MOC_files ${CMAKE_CURRENT_SOURCE_DIR}/abstractunittest.h)
add_library(libmetadataenginetest STATIC ${MOC_files})
target_link_libraries(libmetadataenginetest ${COMMON_TEST_LINK})

macro(METADATAENGINE_TESTS_BUILD)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/${_target}.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore
                  libmetadataenginetest

                  ${COMMON_TEST_LINK}
    )

endmacro()

METADATAENGINE_TESTS_BUILD(applytags_utest.cpp)
METADATAENGINE_TESTS_BUILD(commentreadwrite_utest.cpp)
METADATAENGINE_TESTS_BUILD(createxmpsidecar_utest.cpp)
METADATAENGINE_TESTS_BUILD(erasemetadatatag_utest.cpp)
METADATAENGINE_TESTS_BUILD(loadfromba_utest.cpp)
METADATAENGINE_TESTS_BUILD(metareaderthread_utest.cpp)
METADATAENGINE_TESTS_BUILD(patchpreview_utest.cpp)
METADATAENGINE_TESTS_BUILD(printiteminfo_utest.cpp)
METADATAENGINE_TESTS_BUILD(printmetadata_utest.cpp)
METADATAENGINE_TESTS_BUILD(printtagslist_utest.cpp)
METADATAENGINE_TESTS_BUILD(ratingreadwrite_utest.cpp)
METADATAENGINE_TESTS_BUILD(setiptcpreview_utest.cpp)
METADATAENGINE_TESTS_BUILD(setxmpface_utest.cpp)
METADATAENGINE_TESTS_BUILD(tagsreadwrite_utest.cpp)
METADATAENGINE_TESTS_BUILD(usexmpsidecar_utest.cpp)

add_subdirectory(exiftool)
