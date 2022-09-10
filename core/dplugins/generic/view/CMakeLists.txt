#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_subdirectory(presentation)

# Not yet finalized
add_subdirectory(slideshow)

if(HAVE_OPENGL)
    add_subdirectory(glviewer)
endif()
