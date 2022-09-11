#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

file(GLOB _files *.png)

install(FILES       ${_files}
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/showfoto/data
)
