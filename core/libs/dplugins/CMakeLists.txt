#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

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

    core/dplugin.cpp
    core/dplugingeneric.cpp
    core/dplugineditor.cpp
    core/dpluginrawimport.cpp
    core/dplugindimg.cpp
    core/dpluginauthor.cpp
    core/dpluginaction.cpp

    setup/dpluginloader.cpp
    setup/dpluginloader_p.cpp
    setup/dpluginconfview.cpp
    setup/dpluginconfviewgeneric.cpp
    setup/dpluginconfvieweditor.cpp
    setup/dpluginconfviewdimg.cpp
    setup/dpluginsetup.cpp
    setup/dpluginaboutdlg.cpp

    iface/dinfointerface.cpp
    iface/dmetainfoiface.cpp

    widgets/dplugindialog.cpp
    widgets/dwizardpage.cpp
    widgets/dwizarddlg.cpp
    widgets/dsavesettingswidget.cpp
    widgets/dwizardpage.cpp
    widgets/dpreviewmanager.cpp
    widgets/dpreviewimage.cpp
    widgets/ditemslist.cpp

    webservices/wscomboboxdelegate.cpp
    webservices/wscomboboxintermediate.cpp
    webservices/wstooldialog.cpp
    webservices/wstoolutils.cpp
    webservices/wssettingswidget.cpp
    webservices/wsselectuserdlg.cpp
    webservices/wslogindialog.cpp
    webservices/wsnewalbumdialog.cpp
    webservices/wssettings.cpp
)

#if(ENABLE_QWEBENGINE)
#    set(libdpluginsinterface_SRCS
#        ${libdpluginsinterface_SRCS}
#        webservices/webwidget_qwebengine.cpp
#       )
#else()
#    set(libdpluginsinterface_SRCS
#        ${libdpluginsinterface_SRCS}
#        webservices/webwidget.cpp
#       )
#endif()

# Used by digikamcore
add_library(dpluginsinterface_src OBJECT ${libdpluginsinterface_SRCS})

target_compile_definitions(dpluginsinterface_src
                           PRIVATE
                           digikamcore_EXPORTS
)

# Parse O2 library code and rules at end, and compilation rules remove important Qt definitions.

if(WIN32)
    # NOTE: this O2 export symbols rule must be on top level.
    add_definitions(-DO2_DLL_EXPORT)
endif()

add_subdirectory(webservices)
