#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_subdirectory(geoiface)
add_subdirectory(mapsearches)
add_subdirectory(geomapwrapper)

if(Qt6_FOUND)

    add_subdirectory(marble)

endif()