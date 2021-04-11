#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(ENABLE_QWEBENGINE)
    include_directories($<TARGET_PROPERTY:Qt5::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
    SET(QWEB_BACKEND Qt5::WebEngineWidgets)
else()
    include_directories($<TARGET_PROPERTY:Qt5::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
    SET(QWEB_BACKEND Qt5::WebKitWidgets)
endif()

add_subdirectory(box)
add_subdirectory(dropbox)
#add_subdirectory(facebook)
add_subdirectory(filecopy)
add_subdirectory(flickr)
add_subdirectory(google)
add_subdirectory(imgur)
add_subdirectory(imageshack)
add_subdirectory(inaturalist)
#add_subdirectory(ipfs)
add_subdirectory(mediawiki)
add_subdirectory(onedrive)
add_subdirectory(pinterest)
add_subdirectory(piwigo)

if(Qt5XmlPatterns_FOUND)
    add_subdirectory(rajce)
endif()

add_subdirectory(smugmug)
add_subdirectory(twitter)
add_subdirectory(yandexfotki)

if(KF5KIO_FOUND)
    add_subdirectory(vkontakte)
    add_subdirectory(filetransfer)
endif()

# Not yet finalized
#add_subdirectory(unified)
