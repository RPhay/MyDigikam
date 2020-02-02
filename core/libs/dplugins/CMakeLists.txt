#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::XmlPatterns,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_QWEBENGINE)
    include_directories($<TARGET_PROPERTY:Qt5::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
else()
    include_directories($<TARGET_PROPERTY:Qt5::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(KF5KIO_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::KIOCore,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:KF5::KIOFileWidgets,INTERFACE_INCLUDE_DIRECTORIES>
    )
endif()

set(libdpluginsinterface_SRCS

    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugin.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugingeneric.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugineditor.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dpluginrawimport.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugindimg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dpluginauthor.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dpluginaction.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginloader.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginloader_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginconfview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginconfviewgeneric.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginconfvieweditor.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginconfviewdimg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginsetup.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginaboutdlg.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/iface/dinfointerface.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/iface/dmetainfoiface.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dplugindialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dwizardpage.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dwizarddlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dsavesettingswidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dwizardpage.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dpreviewmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dpreviewimage.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/ditemslist.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wscomboboxdelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wscomboboxintermediate.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wstooldialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wstoolutils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wssettingswidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wsselectuserdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wslogindialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wsnewalbumdialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wssettings.cpp
)

#if(ENABLE_QWEBENGINE)
#    set(libdpluginsinterface_SRCS
#        ${libdpluginsinterface_SRCS}
#        ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/webwidget_qwebengine.cpp
#       )
#else()
#    set(libdpluginsinterface_SRCS
#        ${libdpluginsinterface_SRCS}
#        ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/webwidget.cpp
#       )
#endif()

add_library(dpluginsinterface_src OBJECT ${libdpluginsinterface_SRCS})

# Parse O2 library code and rules at end, and compilation rules remove important Qt definitions.

if(WIN32)
    # NOTE: this O2 export symbols rule must be on top level.
    add_definitions(-DO2_DLL_EXPORT)
endif()

add_subdirectory(webservices)
