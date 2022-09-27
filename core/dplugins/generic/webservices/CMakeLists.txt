#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

include_directories(${CMAKE_SOURCE_DIR}/core/libs/dplugins/webservices/o2/src)

if(ENABLE_QWEBENGINE)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
    set(QWEB_BACKEND Qt${QT_VERSION_MAJOR}::WebEngineWidgets)

else()

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
    set(QWEB_BACKEND Qt${QT_VERSION_MAJOR}::WebKitWidgets)

endif()

add_subdirectory(box)
add_subdirectory(dropbox)
add_subdirectory(filecopy)
add_subdirectory(flickr)
add_subdirectory(google)
add_subdirectory(imgur)
add_subdirectory(imageshack)
add_subdirectory(inaturalist)
add_subdirectory(mediawiki)
add_subdirectory(onedrive)
add_subdirectory(pinterest)
add_subdirectory(piwigo)

# Deprecated
#add_subdirectory(facebook)
#add_subdirectory(ipfs)

if(NOT Qt6_FOUND AND Qt5XmlPatterns_FOUND)

    add_subdirectory(rajce)

endif()

add_subdirectory(smugmug)
add_subdirectory(twitter)

if(KF5KIO_FOUND)

    add_subdirectory(filetransfer)

endif()

# Not yet finalized
#add_subdirectory(unified)
