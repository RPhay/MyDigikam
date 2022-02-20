# Script to build FlowView plugin for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(PREFIX_ext_flowview "${EXTPREFIX}")

if (MINGW)

    ExternalProject_Add(ext_flowview

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cathaysia/digikamflowplugin.git

        CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && make install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

else() # MacOS and Linux

    ExternalProject_Add(ext_flowview

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/cathaysia/digikamflowplugin.git

        INSTALL_DIR ${PREFIX_ext_flowview}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_flowview} -DCMAKE_BUILD_TYPE=RelWithDebInfo

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
    )

endif()
