#
# SPDX-FileCopyrightText: 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

file(GLOB icmfiles *.icm *.icc)

install(FILES  ${icmfiles}
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/digikam/profiles)
