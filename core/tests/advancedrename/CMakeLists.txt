#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(renamecustomizertest_SRCS
    renamecustomizertest.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
)

ecm_add_tests(${renamecustomizertest_SRCS}

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamgui

              ${COMMON_TEST_LINK}
              ${OpenCV_LIBRARIES}
)

#------------------------------------------------------------------------

set(cameranamehelper_SRCS
    cameranamehelpertest.cpp
)

ecm_add_tests(${cameranamehelper_SRCS}

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              ${COMMON_TEST_LINK}
              ${OpenCV_LIBRARIES}
)

#------------------------------------------------------------------------

set(advancedrenametest_SRCS
    advancedrenametest.cpp
)

ecm_add_tests(${advancedrenametest_SRCS}

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              ${COMMON_TEST_LINK}
              ${OpenCV_LIBRARIES}
)
