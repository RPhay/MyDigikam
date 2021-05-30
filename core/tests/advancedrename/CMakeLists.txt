#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/renamecustomizer_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamgui

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/cameranamehelper_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              ${COMMON_TEST_LINK}
)

#------------------------------------------------------------------------

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/advancedrename_utest.cpp

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              ${COMMON_TEST_LINK}
)
