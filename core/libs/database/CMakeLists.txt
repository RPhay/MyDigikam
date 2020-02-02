#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2015      by Mohamed_Anwer, <m_dot_anwer at gmx dot com>
# Copyright (C) 2018      by Mario Frank,   <mario dot frank at uni minus potsdam dot de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Boost uses operator names (and, not, ...)
string(REPLACE "-fno-operator-names" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(libdatabasecore_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/database/server/databaseserverstarter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/server/databaseservererror.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/server/databaseserver.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineconfigloader.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineconfig.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineactiontype.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineerrorhandler.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineguierrorhandler.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineparameters.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbenginebackend.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbenginesqlquery.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/engine/dbengineaccess.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/database/tags/tagregion.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/database/thumbsdb/thumbsdb.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/thumbsdb/thumbsdbschemaupdater.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/thumbsdb/thumbsdbbackend.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/thumbsdb/thumbsdbaccess.cpp
)

set(libdatabaseutils_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/widgets/dbstatdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/widgets/dbsettingswidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/widgets/dbmigrationdlg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/widgets/mysqlinitbinary.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/widgets/mysqlservbinary.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/widgets/searchtextbardb.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/ifaces/dbinfoiface.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/ifaces/syncjob.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/ifaces/dio.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/ifaces/diofinders.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/scan/scancontroller.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/scan/scancontroller_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/scan/scancontroller_start.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/scan/scancontroller_progress.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/utils/scan/scancontroller_stop.cpp
)

set(libdatabasemodels_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemfiltermodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemfiltermodel_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemfiltermodelthreads.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemfiltersettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemversionsmodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemthumbnailmodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemsortsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemlistmodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/models/itemmodel.cpp
)

set(libdatabase_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionscanner.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionscanner_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionscanner_scan.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionscanner_utils.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionmanager_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionmanager_location.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionmanager_album.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionlocation.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/collection/collectionscannerhints.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredb.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbsearchxml.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbchecker.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbcopymanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbconstants.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbtransaction.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbschemaupdater.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbchangesets.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbthumbinfoprovider.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredboperationgroup.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbbackend.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbwatch.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredburl.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbaccess.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbnamefilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbdownloadhistory.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/database/tags/tagproperties.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/tags/tagscache.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/tags/facetags.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/tags/facetagseditor.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/tags/facetagsiface.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/database/dbjobs/dbjobsthread.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/dbjobs/dbjob.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/dbjobs/dbjobinfo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/dbjobs/dbjobsmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/dbjobs/duplicatesprogressobserver.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_containers.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_geolocation.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_groups.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_history.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_labels.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_properties.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_similarity.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_tags.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfo_thumbnail.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfodata.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfolist.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/iteminfocache.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/itemcomments.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/itemcopyright.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/itemposition.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/itemtagpair.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/itemextendedproperties.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlister.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlister_palbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlister_talbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlister_salbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlister_falbum.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlisterrecord.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemlisterreceiver.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/lister/itemattributeswatch.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/query/itemquerybuilder.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/query/itemquerybuilder_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/query/itemqueryposthooks.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/query/fieldquerybuilder.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_p.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_database.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_file.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_photo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_video.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_history.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/item/scanner/itemscanner_baloo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/
    ${CMAKE_SOURCE_DIR}/core/libs/database/history/itemhistorygraph.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/history/itemhistorygraphmodel.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/
    ${CMAKE_SOURCE_DIR}/core/libs/database/similaritydb/similaritydb.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/similaritydb/similaritydbschemaupdater.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/similaritydb/similaritydbbackend.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/similaritydb/similaritydbaccess.cpp
)

if(ENABLE_DBUS)
    set(libdatabase_SRCS
        ${libdatabase_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/database/coredb/coredbwatchadaptor.cpp
       )
endif()

if(Marble_FOUND)
    set(libdatabase_SRCS
        ${libdatabase_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/database/item/containers/itemgps.cpp
       )
endif()

# enable exceptions
kde_source_files_enable_exceptions(${CMAKE_SOURCE_DIR}/core/libs/database/history/itemhistorygraph.cpp
                                   ${CMAKE_SOURCE_DIR}/core/libs/database/history/itemhistorygraphmodel.cpp
)

set(libhaar_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/database/haar/haar.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/haar/haariface.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/database/haar/haariface_p.cpp
)

# Shared libdigikamdatabase ########################################################

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

add_library(digikamdatabasemain_src OBJECT ${libdatabaseutils_SRCS})
add_library(digikamdatabasecore_src OBJECT ${libdatabasecore_SRCS})

add_library(digikamdatabase_src     OBJECT ${libdatabase_SRCS}
                                           ${libdatabasemodels_SRCS}
                                           ${libhaar_SRCS}
)
