#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libcommonwidgets_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/common/abstractitemdragdrophandler.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/common/dactivelabel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/common/dragdropimplementations.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/common/modelcompleter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/common/searchtextbar.cpp
)

set(liblayouttools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/layout/dexpanderbox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/layout/statesavingobject.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/layout/dlayoutbox.cpp
)

set(librangetools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/range/dsliderspinbox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/range/dnuminput.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/range/drangebox.cpp
)

set(libcombotools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/combo/comboboxutilities.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/combo/dcombobox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/combo/squeezedcombobox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/combo/timezonecombobox.cpp
)

set(libcolorstools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/dcolorvalueselector.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/dhuesaturationselect.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/dcolorchoosermode.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/dcolorselector.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/colorgradientwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/dgradientslider.cpp
)

set(libfontstools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/fonts/dfontproperties.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/fonts/dfontselect.cpp
)

set(libfilestools_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/filesaveoptionsbox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/filesaveconflictbox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/dfiledialog.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/dfileselector.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/dbinarysearch.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/dbinaryiface.cpp
)

set(libmainview_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/dxmlguiwindow.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/daboutdata.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/dzoombar.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/dcursortracker.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/thumbbardock.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/thememanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/thememanager_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/sidebar.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/fullscreensettings.cpp
    ${CMAKE_SOURCE_DIR}/core/app/utils/digikam_globals.cpp
)

set(libitemview_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/dcategorizedview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/dcategorizedsortfilterproxymodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/ditemdelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/ditemtooltip.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/dcategorydrawer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/itemdelegateoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/itemviewhoverbutton.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/itemviewdelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/itemviewtooltip.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/itemviewcategorized.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/itemview/actioncategorizedview.cpp
)

set(libgraphicsviewwidgets_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/clickdragreleaseitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/dimgchilditem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/dimgpreviewitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/regionframeitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/graphicsdimgitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/graphicsdimgview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/imagezoomsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/previewlayout.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/paniconwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/itemvisibilitycontroller.cpp
)

set(libmetadatawidgets_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/mdkeylistviewitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/metadatalistview.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/metadatalistviewitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/metadatawidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/metadataselector.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/metadatapanel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/iptcwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/exifwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/makernotewidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/xmpwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/ratingwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/colorlabelwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/picklabelwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/altlangstredit.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/colorlabelwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/countryselector.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/metadata/subjectwidget.cpp
)

set(libiccprofileswidgets_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/iccprofiles/iccpreviewwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/iccprofiles/iccprofilewidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/iccprofiles/cietonguewidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/iccprofiles/iccprofilescombobox.cpp
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

add_library(digikamwidgetscore_src OBJECT
    ${libmainview_SRCS}
    ${libcommonwidgets_SRCS}
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
)

#############################################################"

set(libimagehistory_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/history/filtershistorywidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/history/itemfiltershistoryitemdelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/history/itemfiltershistorymodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/history/itemfiltershistorytreeitem.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/widgets/history/versionswidget.cpp
)

add_library(imagehistorywidgets_src OBJECT ${libimagehistory_SRCS})
