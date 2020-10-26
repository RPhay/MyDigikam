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

# ----------------------------------------------------------------------------------

QT5_WRAP_CPP(MOC_files ${CMAKE_CURRENT_SOURCE_DIR}/abstractunittest.h)
add_library(libmetadatenginetest STATIC ${MOC_files})
target_link_libraries(libmetadatenginetest ${COMMON_TEST_LINK})

macro(METADATAENGINE_TESTS_BUILD)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/${_target}.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore
                  libmetadatenginetest

                  ${COMMON_TEST_LINK}
    )

endmacro()

METADATAENGINE_TESTS_BUILD(tagsreadwritetest.cpp)
METADATAENGINE_TESTS_BUILD(ratingreadwritetest.cpp)
METADATAENGINE_TESTS_BUILD(commentreadwritetest.cpp)
METADATAENGINE_TESTS_BUILD(patchpreviewtest.cpp)
METADATAENGINE_TESTS_BUILD(applytagstest.cpp)
METADATAENGINE_TESTS_BUILD(loadfrombatest.cpp)
METADATAENGINE_TESTS_BUILD(usexmpsidecartest.cpp)
METADATAENGINE_TESTS_BUILD(setiptcpreviewtest.cpp)
METADATAENGINE_TESTS_BUILD(erasemetadatatagtest.cpp)
METADATAENGINE_TESTS_BUILD(printtagslisttest.cpp)
METADATAENGINE_TESTS_BUILD(createxmpsidecartest.cpp)
METADATAENGINE_TESTS_BUILD(setxmpfacetest.cpp)
METADATAENGINE_TESTS_BUILD(printmetadatatest.cpp)
METADATAENGINE_TESTS_BUILD(printiteminfotest.cpp)
METADATAENGINE_TESTS_BUILD(metareaderthreadtest.cpp)
