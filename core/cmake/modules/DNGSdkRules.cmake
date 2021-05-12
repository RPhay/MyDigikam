#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

#---------------------------------

# Common DNG SDK and XMP SDK definitions shared for DNGWriter and RawEngine.
#
# Set platteforms flags.
#
# NOTE: see bug #195735: do not enable Mac flags provided by Adobe.
#       Sounds like all compile fine like under Linux.

add_definitions(-DEnablePluginManager=0)    # XMP SDK
add_definitions(-DXMP_StaticBuild=1)        # XMP SDK
add_definitions(-DqDNGXMPFiles=0)           # DNG SDK: minimum XMP-set
add_definitions(-DqDNGXMPDocOps=0)          # DNG SDK: minimum XMP-set
add_definitions(-DqDNGDebug=0)              # DNG SDK: no debug code
add_definitions(-DqDNGUseLibJPEG=1)         # DNG SDK: use libjpeg
add_definitions(-DqDNGUseStdInt=1)          # DNG SDK: use standard definition of integer
add_definitions(-DqDNGThreadSafe=1)         # DNG SDK: Use thread safety
add_definitions(-DqDNGValidateTarget=1)     # DNG SDK: compilation of DNG validation CLI tool

# Plateform definitions

if(MSVC)

    add_definitions(
                    # XMP SDK
                    -DWIN_ENV=1

                    # DNG SDK
                    -DqWinOS=1
                    -DqMacOS=0
                    -DqLinux=0
    )

else()

    add_definitions(
                     # XMP SDK
                    -DUNIX_ENV=1

                    # DNG SDK
                    -DqWinOS=0
                    -DqMacOS=0
                    -DqLinux=1
    )

endif()

# Check processor endianness

include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

if(NOT IS_BIG_ENDIAN)

    add_definitions(-DqDNGLittleEndian=1)

endif()
