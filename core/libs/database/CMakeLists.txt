#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2015      by Mohamed_Anwer, <m_dot_anwer at gmx dot com>
# Copyright (C) 2018      by Mario Frank,   <mario dot frank at uni minus potsdam dot de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::CoreAddons,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Boost uses operator names (and, not, ...)
string(REPLACE "-fno-operator-names" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

### For digikamcore target - Symbols exported with DIGIKAM_EXPORT macro ##########################

set(libdatabasecore_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/server/databaseserverstarter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server/databaseservererror.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server/databaseserver.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineconfigloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineactiontype.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineerrorhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineguierrorhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineparameters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbenginebackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbenginesqlquery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/dbengineaccess.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/tags/tagregion.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/thumbsdb/thumbsdb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbsdb/thumbsdbschemaupdater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbsdb/thumbsdbbackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbsdb/thumbsdbaccess.cpp
)

# Used by digikamcore
add_library(core_digikamdatabase_obj OBJECT ${libdatabasecore_SRCS})

target_compile_definitions(core_digikamdatabase_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

### For digikamdatabase target - Symbols exported with DIGIKAM_DATABASE_EXPORT macro ##############

set(libdatabase_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionscanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionscanner_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionscanner_scan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionscanner_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionmanager_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionmanager_location.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionmanager_album.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionlocation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collection/collectionscannerhints.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbsearchxml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbchecker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbcopymanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbconstants.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbtransaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbschemaupdater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbchangesets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbthumbinfoprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredboperationgroup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbbackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbwatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredburl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbaccess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbnamefilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbdownloadhistory.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/tags/tagproperties.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags/tagscache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags/facetags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags/facetagseditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags/facetagsiface.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/dbjobs/dbjobsthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbjobs/dbjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbjobs/dbjobinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbjobs/dbjobsmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbjobs/duplicatesprogressobserver.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_containers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_geolocation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_groups.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_history.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_labels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_properties.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_similarity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_tags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfo_thumbnail.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfodata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfolist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/iteminfocache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/itemcomments.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/itemcopyright.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/itemposition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/itemtagpair.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/itemextendedproperties.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlister.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlister_palbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlister_talbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlister_salbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlister_falbum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlisterrecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemlisterreceiver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/lister/itemattributeswatch.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/item/query/itemquerybuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/query/itemquerybuilder_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/query/itemqueryposthooks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/query/fieldquerybuilder.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_photo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_video.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_history.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/item/scanner/itemscanner_baloo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/history/itemhistorygraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/itemhistorygraphmodel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/similaritydb/similaritydb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/similaritydb/similaritydbschemaupdater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/similaritydb/similaritydbbackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/similaritydb/similaritydbaccess.cpp
)

if(ENABLE_DBUS)
    set(libdatabase_SRCS
        ${libdatabase_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/coredb/coredbwatchadaptor.cpp
       )
endif()

if(Marble_FOUND)
    set(libdatabase_SRCS
        ${libdatabase_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/item/containers/itemgps.cpp
       )
endif()

# enable exceptions
kde_source_files_enable_exceptions(${CMAKE_CURRENT_SOURCE_DIR}/history/itemhistorygraph.cpp
                                   ${CMAKE_CURRENT_SOURCE_DIR}/history/itemhistorygraphmodel.cpp
)

set(libdatabasemodels_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemfiltermodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemfiltermodel_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemfiltermodelthreads.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemfiltersettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemversionsmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemthumbnailmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemsortcollator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemsortsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/itemmodel.cpp
)

set(libhaar_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/haar/haar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/haar/haariface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/haar/haariface_p.cpp
)

# Used by digikamdatabase
add_library(digikamdatabase_obj OBJECT ${libdatabase_SRCS}
                                       ${libdatabasemodels_SRCS}
                                       ${libhaar_SRCS}
)

target_compile_definitions(digikamdatabase_obj
                           PRIVATE
                           digikamdatabase_EXPORTS
)

### For digikamgui target - Symbols exported with DIGIKAM_EXPORT macro ############################

set(libdatabasemain_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/dbstatdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/dbsettingswidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/dbmigrationdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/mysqladminbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/mysqlinitbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/mysqlservbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/widgets/searchtextbardb.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/utils/ifaces/dbinfoiface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/ifaces/syncjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/ifaces/dio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/ifaces/diofinders.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/utils/scan/scancontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/scan/scancontroller_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/scan/scancontroller_start.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/scan/scancontroller_progress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/scan/scancontroller_stop.cpp
)

# Used by digikamgui
add_library(gui_digikamdatabasemain_obj OBJECT ${libdatabasemain_SRCS})

target_compile_definitions(gui_digikamdatabasemain_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

