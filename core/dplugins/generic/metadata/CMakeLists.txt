#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_subdirectory(metadataedit)
add_subdirectory(timeadjust)

if(${Marble_FOUND})
    add_subdirectory(geolocationedit)
endif()
