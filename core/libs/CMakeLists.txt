#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# NOTE: include this common rules on top level for DNGWriter and RawEngine.

include(DNGSdkRules)

add_subdirectory(dimg)
add_subdirectory(dragdrop)
add_subdirectory(metadataengine)
add_subdirectory(database)
add_subdirectory(dngwriter)
add_subdirectory(dtrash)
add_subdirectory(facesengine)
add_subdirectory(iojobs)
add_subdirectory(heifutils)
add_subdirectory(jpegutils)
add_subdirectory(pgfutils)
add_subdirectory(threadimageio)
add_subdirectory(widgets)
add_subdirectory(properties)
add_subdirectory(progressmanager)
add_subdirectory(threads)
add_subdirectory(versionmanager)
add_subdirectory(notificationmanager)
add_subdirectory(models)
add_subdirectory(onlineversion)
add_subdirectory(template)
add_subdirectory(dialogs)
add_subdirectory(dplugins)
add_subdirectory(kmemoryinfo)
add_subdirectory(fileactionmanager)
add_subdirectory(filters)
add_subdirectory(settings)
add_subdirectory(rawengine)
add_subdirectory(album)
add_subdirectory(tags)
add_subdirectory(transitionmngr)
add_subdirectory(timeadjust)

if(ENABLE_MEDIAPLAYER)
    add_subdirectory(video)
endif()
