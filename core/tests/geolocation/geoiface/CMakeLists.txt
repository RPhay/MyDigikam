#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
)

add_subdirectory(calibrator)
add_subdirectory(demo)

# -- test the primitive datatypes and helper functions -----------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_primitives.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

# -- test the GeoCoordinates class --------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_geocoordinates.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

# -- test the TileIndex class --------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_tileindex.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

# -- test the LookupAltitudeGeonames class -------------------------------------------------------

# do not add this as a test because it only works if there is an internet connection
#
#ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_lookup_altitude_geonames.cpp
#
#              NAME_PREFIX
#
#              "digikam-"
#
#              LINK_LIBRARIES
#
#              digikamcore
#
#              ${COMMON_TEST_LINK}
#)

# -- test the marker model ------------------------------------------------------------------------

if(NOT WIN32)

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_itemmarkertiler.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore

                  ${COMMON_TEST_LINK}
    )

endif()

# -- test the track management classes -------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_tracks.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

# -- track loading application for timing tests ----------------------------------------------------

set(loadtrack_sources ${CMAKE_CURRENT_SOURCE_DIR}/loadtrack.cpp)

add_executable(loadtrack ${loadtrack_sources})

target_link_libraries(loadtrack

                      digikamcore

                      ${COMMON_TEST_LINK}
)
