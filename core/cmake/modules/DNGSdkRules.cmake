#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

#---------------------------------

# Common DNG SDK and XMP SDK definitions shared for DNGWriter and RawEngine.
#
# NOTE: see bug #195735: for platteforms flags, do not enable Mac flags provided by Adobe.
#       Sounds like all compile fine like under Linux.

# --- For XMP SDK ---------------------------------------------------------------------

add_definitions(-DEnablePluginManager=0)    # Do not use plugin extensions
add_definitions(-DXMP_StaticBuild=1)        # Build statically

# Prevent a denial-service-attack related to XML entity expansion
# ("billion laughs attack").
# See https://bugzilla.redhat.com/show_bug.cgi?id=888769

add_definitions(-DBanAllEntityUsage=1)

# Platteform definitions

if(MSVC)

    add_definitions(-DWIN_ENV=1)

else()

    add_definitions(-DUNIX_ENV=1)

endif()

# --- For DNG SDK ----------------------------------------------------------------------

add_definitions(-DqDNGXMPFiles=0)           # Minimum XMP-set
add_definitions(-DqDNGXMPDocOps=0)          # Minimum XMP-set
add_definitions(-DqDNGDebug=0)              # No debug code
add_definitions(-DqDNGUseLibJPEG=1)         # Use libjpeg
add_definitions(-DqDNGUseStdInt=1)          # Use standard definition of integer
add_definitions(-DqDNGThreadSafe=1)         # Use thread safety
add_definitions(-DqDNGValidateTarget=1)     # Compilation of DNG validation CLI tool

# Platteform definitions

if(MSVC)

    add_definitions(-DqWinOS=1
                    -DqMacOS=0
                    -DqLinux=0)

else()

    add_definitions(-DqWinOS=0
                    -DqMacOS=0
                    -DqLinux=1)

endif()

# Check processor endianness

include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

if(NOT IS_BIG_ENDIAN)

    add_definitions(-DqDNGLittleEndian=1)

endif()
