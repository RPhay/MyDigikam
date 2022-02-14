#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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
