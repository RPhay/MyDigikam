#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# digiKam exported API for external DPlugins based projects.
#
# Note: all headers must export only Qt5 dependencies.
# No KF5 dependencies are permitted: all external DPlugin based projects do not depend explicitly to KF5 API.
# External DPlugins demo code can be found in this project: https://github.com/cgilles/digikamplugins-demo

# -----------------------------------------------------------------------------------
# Headers to install

install(FILES

              # As part of DigikamCore

              ${CMAKE_BINARY_DIR}/core/app/utils/digikam_config.h
              ${CMAKE_BINARY_DIR}/core/app/utils/digikam_core_export.h
              ${CMAKE_BINARY_DIR}/core/app/utils/digikam_database_export.h
              ${CMAKE_BINARY_DIR}/core/app/utils/digikam_gui_export.h

              ${CMAKE_SOURCE_DIR}/core/app/utils/digikam_export.h
              ${CMAKE_SOURCE_DIR}/core/app/utils/digikam_globals.h

              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugin.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dpluginaction.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dpluginauthor.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugineditor.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dplugingeneric.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/core/dpluginrawimport.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginloader.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/setup/dpluginaboutdlg.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/iface/dinfointerface.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/iface/dmetainfoiface.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/ditemslist.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dplugindialog.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dpreviewimage.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dpreviewmanager.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dsavesettingswidget.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dwizarddlg.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/widgets/dwizardpage.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wstooldialog.h
              ${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/wstoolutils.h

              ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/items/gpsitemcontainer.h
              ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/correlator/gpsdatacontainer.h
              ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/core/geoifacetypes.h
              ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/core/geocoordinates.h
              ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/reversegeocoding/rginfo.h

              ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/core/iofilesettings.h
              ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/editor/imageiface.h
              ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/editor/editortool.h
              ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/widgets/imageregionwidget.h
              ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/widgets/previewtoolbar.h

              ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/color/dcolor.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/color/dcolorpixelaccess.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/color/dcolorcomposer.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/color/dcolorblend.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/dimgloaderobserver.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/history/historyimageid.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/history/filteraction.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgbuiltinfilter.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgthreadedfilter.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgthreadedanalyser.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/iccprofile.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/icctransform.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bcg/bcgcontainer.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/wb/wbcontainer.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/curvescontainer.h
              ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/raw/drawdecoding.h

              ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine.h
              ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/engine/metaengine_data.h
              ${CMAKE_SOURCE_DIR}/core/libs/metadataengine/containers/photoinfocontainer.h

              ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawdecoder.h
              ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawinfo.h
              ${CMAKE_SOURCE_DIR}/core/libs/rawengine/drawdecodersettings.h

              ${CMAKE_SOURCE_DIR}/core/libs/widgets/mainview/dactivelabel.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/files/filesaveconflictbox.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/colors/dcolorselector.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/imagezoomsettings.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/previewlayout.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/dimgpreviewitem.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/graphicsdimgview.h
              ${CMAKE_SOURCE_DIR}/core/libs/widgets/graphicsview/graphicsdimgitem.h

              ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/preview/previewsettings.h
              ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadingdescription.h
              ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/fileio/loadsavethread.h
              ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailinfo.h
              ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/thumb/thumbnailloadthread.h
              ${CMAKE_SOURCE_DIR}/core/libs/threadimageio/engine/managedloadsavethread.h
              ${CMAKE_SOURCE_DIR}/core/libs/threads/dynamicthread.h

              ${CMAKE_SOURCE_DIR}/core/libs/dialogs/dmessagebox.h
              ${CMAKE_SOURCE_DIR}/core/libs/dialogs/imagedialog.h

              ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/dhistoryview.h
              ${CMAKE_SOURCE_DIR}/core/libs/progressmanager/dprogresswdg.h

              # As part of DigikamDatabase

              ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfolist.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbfields.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbconstants.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbalbuminfo.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbaccess.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredburl.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineparameters.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineaction.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineconfig.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineerrorhandler.h
              ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineconfigsettings.h

              # As part of DigikamGui

              ${CMAKE_SOURCE_DIR}/core/utilities/queuemanager/dplugins/dpluginbqm.h
              ${CMAKE_SOURCE_DIR}/core/utilities/queuemanager/manager/batchtool.h
              ${CMAKE_SOURCE_DIR}/core/utilities/queuemanager/manager/queuesettings.h

        DESTINATION

              ${CMAKE_INSTALL_INCLUDEDIR}/digikam
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/templates/DigikamPluginConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/DigikamPluginConfig.cmake)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DigikamPluginConfig.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamPlugin")
