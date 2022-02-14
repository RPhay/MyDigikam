#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

install(PROGRAMS digitaglinktree/digitaglinktree
                 cleanup_digikamdb/cleanup_digikamdb
        DESTINATION ${KDE_INSTALL_FULL_BINDIR}
)

install(FILES digitaglinktree/digitaglinktree.1
              cleanup_digikamdb/cleanup_digikamdb.1
        DESTINATION ${KDE_INSTALL_FULL_MANDIR}/man1
)

