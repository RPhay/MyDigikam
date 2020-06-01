#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

# ----------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_correlator.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

# ----------------------------------------------------------------

if(NOT WIN32)

    ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_rgparsing.cpp

                  NAME_PREFIX

                  "digikam-"

                  LINK_LIBRARIES

                  digikamcore

                  ${COMMON_TEST_LINK}
    )

endif()

# ----------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_gpsimageitem.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore

              ${COMMON_TEST_LINK}
)

# ----------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_rgtagmodel.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase

              libmodeltest

              ${COMMON_TEST_LINK}
)

# ----------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/test_simpletreemodel.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase

              libmodeltest

              ${COMMON_TEST_LINK}
)
