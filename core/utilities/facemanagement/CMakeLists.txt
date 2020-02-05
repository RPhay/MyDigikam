#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libfacemanagement_SRCS
    database/faceutils.cpp
    database/faceitemretriever.cpp
    database/facescansettings.cpp
    items/faceitem.cpp
    items/facegroup.cpp
    items/facegroup_p.cpp
    threads/facepreviewloader.cpp
    threads/facepipeline.cpp
    threads/facepipeline_p.cpp
    threads/facepipelinepackage.cpp
    threads/parallelpipes.cpp
    threads/scanstatefilter.cpp
    widgets/facescanwidget.cpp
    widgets/assignnamewidget.cpp
    widgets/assignnamewidget_p.cpp
    widgets/assignnamewidgetstates.cpp
    bench/detectionbenchmarker.cpp
    bench/recognitionbenchmarker.cpp
    workers/detectionworker.cpp
    workers/recognitionworker.cpp
    workers/databasewriter.cpp
    workers/trainerworker.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(facemanagement_src OBJECT ${libfacemanagement_SRCS})

target_compile_definitions(facemanagement_src
                           PRIVATE
                           digikamgui_EXPORTS
)
