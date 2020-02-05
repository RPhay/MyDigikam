#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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
    server/databaseserverstarter.cpp
    server/databaseservererror.cpp
    server/databaseserver.cpp

    engine/dbengineconfigloader.cpp
    engine/dbengineconfig.cpp
    engine/dbengineactiontype.cpp
    engine/dbengineerrorhandler.cpp
    engine/dbengineguierrorhandler.cpp
    engine/dbengineparameters.cpp
    engine/dbenginebackend.cpp
    engine/dbenginesqlquery.cpp
    engine/dbengineaccess.cpp

    tags/tagregion.cpp

    thumbsdb/thumbsdb.cpp
    thumbsdb/thumbsdbschemaupdater.cpp
    thumbsdb/thumbsdbbackend.cpp
    thumbsdb/thumbsdbaccess.cpp
)

### For digikamgui target - Symbols exported with DIGIKAM_EXPORT macro ############################

set(libdatabaseutils_SRCS
    utils/widgets/dbstatdlg.cpp
    utils/widgets/dbsettingswidget.cpp
    utils/widgets/dbmigrationdlg.cpp
    utils/widgets/mysqlinitbinary.cpp
    utils/widgets/mysqlservbinary.cpp
    utils/widgets/searchtextbardb.cpp

    utils/ifaces/dbinfoiface.cpp
    utils/ifaces/syncjob.cpp
    utils/ifaces/dio.cpp
    utils/ifaces/diofinders.cpp

    utils/scan/scancontroller.cpp
    utils/scan/scancontroller_p.cpp
    utils/scan/scancontroller_start.cpp
    utils/scan/scancontroller_progress.cpp
    utils/scan/scancontroller_stop.cpp
)

### For digikamdatabase target - Symbols exported with DIGIKAM_DATABASE_EXPORT macro ##############

set(libdatabase_SRCS
    collection/collectionscanner.cpp
    collection/collectionscanner_p.cpp
    collection/collectionscanner_scan.cpp
    collection/collectionscanner_utils.cpp
    collection/collectionmanager.cpp
    collection/collectionmanager_p.cpp
    collection/collectionmanager_location.cpp
    collection/collectionmanager_album.cpp
    collection/collectionlocation.cpp
    collection/collectionscannerhints.cpp

    coredb/coredb.cpp
    coredb/coredbsearchxml.cpp
    coredb/coredbchecker.cpp
    coredb/coredbcopymanager.cpp
    coredb/coredbconstants.cpp
    coredb/coredbtransaction.cpp
    coredb/coredbschemaupdater.cpp
    coredb/coredbchangesets.cpp
    coredb/coredbthumbinfoprovider.cpp
    coredb/coredboperationgroup.cpp
    coredb/coredbbackend.cpp
    coredb/coredbwatch.cpp
    coredb/coredburl.cpp
    coredb/coredbaccess.cpp
    coredb/coredbnamefilter.cpp
    coredb/coredbdownloadhistory.cpp

    tags/tagproperties.cpp
    tags/tagscache.cpp
    tags/facetags.cpp
    tags/facetagseditor.cpp
    tags/facetagsiface.cpp

    dbjobs/dbjobsthread.cpp
    dbjobs/dbjob.cpp
    dbjobs/dbjobinfo.cpp
    dbjobs/dbjobsmanager.cpp
    dbjobs/duplicatesprogressobserver.cpp

    item/containers/iteminfo.cpp
    item/containers/iteminfo_p.cpp
    item/containers/iteminfo_containers.cpp
    item/containers/iteminfo_geolocation.cpp
    item/containers/iteminfo_groups.cpp
    item/containers/iteminfo_history.cpp
    item/containers/iteminfo_labels.cpp
    item/containers/iteminfo_properties.cpp
    item/containers/iteminfo_similarity.cpp
    item/containers/iteminfo_tags.cpp
    item/containers/iteminfo_thumbnail.cpp
    item/containers/iteminfodata.cpp
    item/containers/iteminfolist.cpp
    item/containers/iteminfocache.cpp
    item/containers/itemcomments.cpp
    item/containers/itemcopyright.cpp
    item/containers/itemposition.cpp
    item/containers/itemtagpair.cpp
    item/containers/itemextendedproperties.cpp

    item/lister/itemlister.cpp
    item/lister/itemlister_palbum.cpp
    item/lister/itemlister_talbum.cpp
    item/lister/itemlister_salbum.cpp
    item/lister/itemlister_falbum.cpp
    item/lister/itemlisterrecord.cpp
    item/lister/itemlisterreceiver.cpp
    item/lister/itemattributeswatch.cpp

    item/query/itemquerybuilder.cpp
    item/query/itemquerybuilder_p.cpp
    item/query/itemqueryposthooks.cpp
    item/query/fieldquerybuilder.cpp

    item/scanner/itemscanner.cpp
    item/scanner/itemscanner_p.cpp
    item/scanner/itemscanner_database.cpp
    item/scanner/itemscanner_file.cpp
    item/scanner/itemscanner_photo.cpp
    item/scanner/itemscanner_video.cpp
    item/scanner/itemscanner_history.cpp
    item/scanner/itemscanner_baloo.cpp

    history/itemhistorygraph.cpp
    history/itemhistorygraphmodel.cpp

    similaritydb/similaritydb.cpp
    similaritydb/similaritydbschemaupdater.cpp
    similaritydb/similaritydbbackend.cpp
    similaritydb/similaritydbaccess.cpp
)

if(ENABLE_DBUS)
    set(libdatabase_SRCS
        ${libdatabase_SRCS}
        coredb/coredbwatchadaptor.cpp
       )
endif()

if(Marble_FOUND)
    set(libdatabase_SRCS
        ${libdatabase_SRCS}
        item/containers/itemgps.cpp
       )
endif()

# enable exceptions
kde_source_files_enable_exceptions(history/itemhistorygraph.cpp
                                   history/itemhistorygraphmodel.cpp
)

set(libdatabasemodels_SRCS
    models/itemfiltermodel.cpp
    models/itemfiltermodel_p.cpp
    models/itemfiltermodelthreads.cpp
    models/itemfiltersettings.cpp
    models/itemversionsmodel.cpp
    models/itemthumbnailmodel.cpp
    models/itemsortsettings.cpp
    models/itemlistmodel.cpp
    models/itemmodel.cpp
)

set(libhaar_SRCS
    haar/haar.cpp
    haar/haariface.cpp
    haar/haariface_p.cpp
)

### Shared digiKam database objects definitions ########################################################

add_library(digikamdatabasecore_src OBJECT ${libdatabasecore_SRCS})             # hosted by digikamcore target.

target_compile_definitions(digikamdatabasecore_src
                           PRIVATE
                           digikamcore_EXPORTS
)


add_library(digikamdatabase_src     OBJECT ${libdatabase_SRCS}                  # hosted by digikamdatabase target.
                                           ${libdatabasemodels_SRCS}
                                           ${libhaar_SRCS}
)

target_compile_definitions(digikamdatabase_src
                           PRIVATE
                           digikamdatabase_EXPORTS
)


add_library(digikamdatabasemain_src OBJECT ${libdatabaseutils_SRCS})            # hosted by digikamgui target.

target_compile_definitions(digikamdatabasemain_src
                           PRIVATE
                           digikamcore_EXPORTS
)
