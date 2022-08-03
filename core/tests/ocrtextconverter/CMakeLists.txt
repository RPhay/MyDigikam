#
# Copyright (c) 2010-2022, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()


include_directories(
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::TextWidgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::CoreAddons,INTERFACE_INCLUDE_DIRECTORIES>
)



add_executable(ocrTesseract_cli  ${CMAKE_CURRENT_SOURCE_DIR}/ocrTesseract_cli.cpp)
target_link_libraries(ocrTesseract_cli

                      textconverterbackend
                      digikamcore
                                            
                      ${COMMON_TEST_LINK}
)

add_executable(ocrTesseract_regexp_test   ${CMAKE_CURRENT_SOURCE_DIR}/ocrTesseract_regexp_test.cpp)
target_link_libraries(ocrTesseract_regexp_test

                      textconverterbackend
                      digikamcore
                                            
                      ${COMMON_TEST_LINK}
)


add_executable(ocrTesseract_process   ${CMAKE_CURRENT_SOURCE_DIR}/ocrTesseract_process.cpp)
target_link_libraries(ocrTesseract_process

                      textconverterbackend
                      digikamcore
                                            
                      ${COMMON_TEST_LINK}
)