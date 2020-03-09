#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)


set(libsetupshowfoto_SRCS
    editor/setupeditoriface.cpp
    editor/setupiofiles.cpp
    editor/setupraw.cpp
    setupicc.cpp
)

# Used by digikamcore
add_library(core_setupcommon_obj OBJECT  ${libsetupshowfoto_SRCS})

target_compile_definitions(core_setupcommon_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# ---------------------------------------------------------------------

set(libsetup_SRCS
    setup.cpp
    setuptooltip.cpp
    setuptemplate.cpp
    setupmisc.cpp
    setupplugins.cpp
    setuplighttable.cpp
    setupdatabase.cpp
    setupimagequalitysorter.cpp
    setuputils.cpp

    editor/setupeditor.cpp
    editor/setupversioning.cpp

    album/setupalbumview.cpp
    album/setupcategory.cpp
    album/setupmime.cpp

    metadata/setupmetadata.cpp
    metadata/advancedmetadatatab.cpp
    metadata/namespacelistview.cpp
    metadata/namespaceeditdlg.cpp

    collections/setupcollections.cpp
    collections/setupcollectionview.cpp
    collections/dwitemdelegate.cpp
    collections/dwitemdelegate_p.cpp
    collections/dwitemdelegatepool.cpp

    camera/importfilterdlg.cpp
    camera/setupcamera.cpp
    camera/cameraselection.cpp
    camera/cameralist.cpp
    camera/cameratype.cpp
)

# Used by digikamgui
add_library(gui_setup_obj OBJECT ${libsetup_SRCS})

target_compile_definitions(gui_setup_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
