# Finds the GDI+ (Graphics Device Interface) on Windows (https://en.wikipedia.org/wiki/Graphics_Device_Interface)
#
# This will define the following variables:
#
#  GDIPLUS_INCLUDE_DIR - where to find gdiplus.h
#  GDIPLUS_LIBRARIES   - List of libraries when using GDI+.
#  GDIPLUS_FOUND       - True if GDI+ found.
#
# SPDX-FileCopyrightText : 2021-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# SPDX-License-Identifier: BSD-3-Clause
#

if(GDIPLUS_INCLUDE_DIR)

    # Already in cache, be silent
    set(GDIPLUS_FIND_QUIETLY TRUE)

endif()

macro(check_winsdk_root_dir key)

  get_filename_component(CANDIDATE ${key} ABSOLUTE)

  if (EXISTS ${CANDIDATE})

    set(WINSDK_ROOT_DIR ${CANDIDATE})

  endif()

endmacro()

check_winsdk_root_dir("[HKEY_LOCAL_MACHINE\\\\SOFTWARE\\\\Microsoft\\\\Microsoft SDKs\\\\Windows\\\\v7.0;InstallationFolder]")
check_winsdk_root_dir("[HKEY_LOCAL_MACHINE\\\\SOFTWARE\\\\Microsoft\\\\Microsoft SDKs\\\\Windows\\\\v7.0A;InstallationFolder]")
check_winsdk_root_dir("[HKEY_LOCAL_MACHINE\\\\SOFTWARE\\\\Microsoft\\\\Microsoft SDKs\\\\Windows\\\\v7.1;InstallationFolder]")
check_winsdk_root_dir("[HKEY_LOCAL_MACHINE\\\\SOFTWARE\\\\Microsoft\\\\Microsoft SDKs\\\\Windows\\\\v7.1A;InstallationFolder]")
check_winsdk_root_dir("[HKEY_LOCAL_MACHINE\\\\SOFTWARE\\\\Microsoft\\\\Windows Kits\\\\Installed Roots;KitsRoot]")
check_winsdk_root_dir("[HKEY_LOCAL_MACHINE\\\\SOFTWARE\\\\Microsoft\\\\Windows Kits\\\\Installed Roots;KitsRoot81]")

find_path(GDIPLUS_INCLUDE_DIR
    NAMES
        GdiPlus.h
        gdiplus.h
    PATH_SUFFIXES
        Include
        Include/um
        Include/shared
    PATHS
        "${WINSDK_ROOT_DIR}"
)

if(EXISTS ${GDIPLUS_INCLUDE_DIR}/GdiPlus.h)

    set(GDIPLUS_LOWERCASE 0 CACHE INTERNAL "Is GdiPlus.h spelt with lowercase?")

else()

    set(GDIPLUS_LOWERCASE 1 CACHE INTERNAL "Is GdiPlus.h spelt with lowercase?")

endif()

if (MINGW)

    find_library(GDIPLUS_LIBRARY NAMES libgdiplus gdiplus)

else()

    set(GDIPLUS_LIBRARY gdiplus)

endif()

# Handle the QUIETLY and REQUIRED arguments and set GDIPLUS_FOUND to TRUE if
# all listed variables are TRUE.

include(FindPackageHandleStandardArgs)
set(FPHSA_NAME_MISMATCHED TRUE)

find_package_handle_standard_args(GDIPLUS DEFAULT_MSG
                                  GDIPLUS_INCLUDE_DIR
                                  GDIPLUS_LIBRARY
)

if(GDIPLUS_FOUND)

    set(GDIPLUS_LIBRARIES ${GDIPLUS_LIBRARY})

else()

    set(GDIPLUS_LIBRARIES)

endif()

mark_as_advanced(GDIPLUS_INCLUDE_DIR GDIPLUS_LIBRARY GDIPLUS_LOWERCASE)
