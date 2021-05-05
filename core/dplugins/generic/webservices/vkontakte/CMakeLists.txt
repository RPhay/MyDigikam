#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2018      by Maik Qualmann, <metzpinguin at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::KIOCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_QWEBENGINE)
    include_directories($<TARGET_PROPERTY:Qt5::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
else()
    include_directories($<TARGET_PROPERTY:Qt5::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# --------------------------------------------------------------------------------

set(libvkontakte_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_vkapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_getapplicationpermissionsjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_authenticationdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_jobs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_userinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_userinfojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_albuminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_albumlistjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_createalbumjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_editalbumjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_deletealbumjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photoinfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photolistjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_getphotouploadserverjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_photopostjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_savephotojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/vkontakte_uploadphotosjob.cpp

# Not used (old implementation or not ported)
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_notejob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_cidsnamesjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_messageinfo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_discussionslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_messageslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_noteinfo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_noteaddjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_allmessageslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_groupinfo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_allnoteslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_noteslistjob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/attic/vkontakte_friendlistjob.cpp

# Deprecated classes
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/deprecated/vkontakte_getvariablejob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/deprecated/vkontakte_getinfojob.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/backend/deprecated/vkontakte_grouplistjob.cpp
)

# For unit-tests and plugin
add_library(vkontaktebackend STATIC ${libvkontakte_SRCS})

# --------------------------------------------------------------------------------

set(vkontakteplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/vkalbumchooser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vkauthwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vknewalbumdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vkplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vkwindow.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    VKontakte
                           SOURCES ${vkontakteplugin_SRCS}
                           DEPENDS vkontaktebackend ${QWEB_BACKEND} KF5::KIOCore
)
