#
# SPDX-FileCopyrightText: 2015-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_subdirectory(autocrop)
add_subdirectory(freerotation)
add_subdirectory(resize)
add_subdirectory(perspective)
add_subdirectory(ratiocrop)
add_subdirectory(shear)

if(LQR-1_FOUND)
    add_subdirectory(contentawareresize)
endif()
