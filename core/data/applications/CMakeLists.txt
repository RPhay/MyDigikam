#
# SPDX-FileCopyrightText: 2010-2024, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

file(GLOB desktopfiles *.desktop)

install(FILES  ${desktopfiles}
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/applications)
