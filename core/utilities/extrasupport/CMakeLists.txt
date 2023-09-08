#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if(KF${QT_VERSION_MAJOR}AkonadiContact_FOUND)
    add_subdirectory(addressbook)
endif()

if(KF${QT_VERSION_MAJOR}FileMetaData_FOUND)
    add_subdirectory(filesindexer)
endif()
