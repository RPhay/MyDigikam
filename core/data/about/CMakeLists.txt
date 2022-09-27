#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

install(FILES main.html
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/digikam/about
)

install(DIRECTORY css/
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/digikam/about/css
)

install(DIRECTORY images/
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/digikam/about/images
)

install(DIRECTORY js/
        DESTINATION ${KDE_INSTALL_FULL_DATADIR}/digikam/about/js
)
