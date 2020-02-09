#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

set(albummodeltest_SRCS
    albummodeltest.cpp
)

ecm_add_tests(${albummodeltest_SRCS}

              NAME_PREFIX

              "digikam-"

              LINK_LIBRARIES

              digikamcore
              digikamdatabase
              digikamgui

              libmodeltest

              ${COMMON_TEST_LINK}
              ${OpenCV_LIBRARIES}
)
