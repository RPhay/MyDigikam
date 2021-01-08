#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libtextwidgets_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/text/modelcompleter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/text/searchtextbar.cpp
)

set(liblayouttools_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/layout/dexpanderbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/layout/statesavingobject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/layout/dlayoutbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/layout/sidebar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/layout/thumbbardock.cpp
)

set(librangetools_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/range/dsliderspinbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/range/dnuminput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/range/drangebox.cpp
)

set(libcombotools_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/combo/comboboxutilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/combo/dcombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/combo/squeezedcombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/combo/timezonecombobox.cpp
)

set(libcolorstools_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/colors/dcolorvalueselector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colors/dhuesaturationselect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colors/dcolorchoosermode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colors/dcolorselector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colors/colorgradientwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colors/dgradientslider.cpp
)

set(libfontstools_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/dfontproperties.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/dfontselect.cpp
)

set(libfilestools_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/files/filesaveoptionsbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/filesaveconflictbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/dfiledialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/dfileselector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/dbinarysearch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/dbinaryiface.cpp
)

set(libmainview_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/dxmlguiwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/daboutdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/dactivelabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/dzoombar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/dcursortracker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/thememanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/thememanager_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainview/fullscreensettings.cpp
    ${CMAKE_SOURCE_DIR}/core/app/utils/digikam_globals.cpp
)

set(libitemview_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/dcategorizedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/dcategorizedsortfilterproxymodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/ditemdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/ditemtooltip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/dcategorydrawer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/itemdelegateoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/itemviewhoverbutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/itemviewdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/itemviewtooltip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/itemviewcategorized.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itemview/actioncategorizedview.cpp
)

set(libgraphicsviewwidgets_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/clickdragreleaseitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/dimgchilditem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/dimgpreviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/regionframeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/graphicsdimgitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/graphicsdimgview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/imagezoomsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/previewlayout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/paniconwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview/itemvisibilitycontroller.cpp
)

set(libmetadatawidgets_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/mdkeylistviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/metadatalistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/metadatalistviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/metadatawidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/metadataselector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/metadatapanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/iptcwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/exifwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/makernotewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/xmpwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/ratingwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/colorlabelwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/picklabelwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/altlangstredit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/colorlabelwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/countryselector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/subjectwidget.cpp
)

set(libiccprofileswidgets_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/iccprofiles/iccpreviewwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iccprofiles/iccprofilewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iccprofiles/cietonguewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iccprofiles/iccprofilescombobox.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5NotifyConfig_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::NotifyConfig,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(Gphoto2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

# Used by digikamcore
add_library(core_digikamwidgets_obj OBJECT
    ${libmainview_SRCS}
    ${libgraphicsviewwidgets_SRCS}
    ${libiccprofileswidgets_SRCS}
    ${libitemview_SRCS}
    ${libmetadatawidgets_SRCS}
    ${libcolorstools_SRCS}
    ${libfontstools_SRCS}
    ${libfilestools_SRCS}
    ${libcombotools_SRCS}
    ${librangetools_SRCS}
    ${liblayouttools_SRCS}
    ${libtextwidgets_SRCS}
)

target_compile_definitions(core_digikamwidgets_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

#############################################################

set(libimagehistory_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/history/filtershistorywidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/itemfiltershistoryitemdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/itemfiltershistorymodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/itemfiltershistorytreeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/versionswidget.cpp
)

# Used by digikamgui
add_library(gui_imagehistorywidgets_obj OBJECT ${libimagehistory_SRCS})

target_compile_definitions(gui_imagehistorywidgets_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
