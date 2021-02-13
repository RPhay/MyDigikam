#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(CMAKE_MIN_VERSION   "3.3.2")
set(ECM_MIN_VERSION     "5.5.0")

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

execute_process(COMMAND ${CMAKE_COMMAND} --version OUTPUT_VARIABLE VERSION_CMAKE_INFO)
message(STATUS "CMake version: ${VERSION_CMAKE_INFO}")

# ==============================================================================
# Information to update before to release this package.

# digiKam version
set(DIGIKAM_MAJOR_VERSION "7")
set(DIGIKAM_MINOR_VERSION "2")
set(DIGIKAM_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final release. Can be used in production.
set(DIGIKAM_SUFFIX_VERSION "")

# ==============================================================================
# Set env. variables accordingly.

# NOTE: This string is used to set libdigikamcore and libdigikamdatabase SO version ID
set(DIGIKAM_VERSION_SHORT
    "${DIGIKAM_MAJOR_VERSION}.${DIGIKAM_MINOR_VERSION}.${DIGIKAM_PATCH_VERSION}"
)

set(DIGIKAM_VERSION_STRING
    "${DIGIKAM_VERSION_SHORT}${DIGIKAM_SUFFIX_VERSION}"
)

# Core Database XML version
# We must set this variable here at top level because it is used in both
# libs/database/core and data/database
# Version history:
# 1 : Original database XML file, published in production.
# 2 : 08-08-2014 : Fix Images.names field size (see bug #327646).
# 3 : 05/11/2015 : Add Face DB schema.
set(DBCORECONFIG_XML_VERSION "3")

# ==============================================================================

project(digikam
        VERSION ${DIGIKAM_VERSION_SHORT}
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for ${PROJECT_NAME}")
message(STATUS "")

### ECM setup #########################################################################################################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/core/cmake/modules ${ECM_MODULE_PATH})

# Cmake macros
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CheckFunctionExists)
include(FeatureSummary)

# ECM macros
include(ECMOptionalAddSubdirectory)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMPoQmTools)
include(ECMAddTests)
include(ECMMarkAsTest)

# KF5 Framework macros
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

# Local macros
include(MacroUtils)
include(MacroCompiler)
include(MacroOpenCV)
include(MacroJPEG)
include(MacroBoolTo01)

#######################################################################################################################

APPLY_COMMON_POLICIES()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Generated")

### COMPILATION OPTIONS MANAGEMENT ####################################################################################

# doc and translations options

option(DIGIKAMSC_CHECKOUT_PO             "Checkout application translations files from git repositories (default=OFF)"            OFF)
option(DIGIKAMSC_CHECKOUT_DOC            "Checkout documentation translations files from git repositories (default=OFF)"          OFF)
option(DIGIKAMSC_COMPILE_PO              "Build translations files (default=ON)"                                                  ON)
option(DIGIKAMSC_COMPILE_DOC             "Build handbook files (default=ON)"                                                      ON)
option(DIGIKAMSC_COMPILE_DIGIKAM         "Build digiKam core (default=ON)"                                                        ON)

#######################################################################################################################

if(DIGIKAMSC_CHECKOUT_PO OR DIGIKAMSC_CHECKOUT_DOC)

    find_package(Ruby)
    find_package(Subversion)

    if(RUBY_EXECUTABLE AND Subversion_FOUND)

        if(DIGIKAMSC_CHECKOUT_PO)

            message(STATUS "Extract application translation files from git repositories. Please wait, it can take a while...")

            execute_process(COMMAND ${RUBY_EXECUTABLE} "${CMAKE_SOURCE_DIR}/project/release/fetch_l10n_po.rb"
                            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

            message(STATUS "Application translation files extraction done.")

        endif()

        if(DIGIKAMSC_CHECKOUT_DOC)

            message(STATUS "Extract documentation translation files from git repositories. Please wait, it can take a while...")
            message(STATUS "NOTE: (u) want mean uncomplete translations files, which will be removed from compilation.")

            execute_process(COMMAND ${RUBY_EXECUTABLE} "${CMAKE_SOURCE_DIR}/project/release/fetch_l10n_doc.rb"
                            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

            message(STATUS "Documentation translation files extraction done.")

        endif()

    else()

        message(FATAL_ERROR "Could not find Ruby or Subversion to extract translation files")

    endif()

endif()

if(DIGIKAMSC_COMPILE_PO AND NOT EXISTS ${CMAKE_SOURCE_DIR}/po/)

    message(STATUS "Application translations directory is missing. Compilation of application translations is disabled...")

    if(NOT DIGIKAMSC_CHECKOUT_PO)
        message(STATUS "You can use DIGIKAMSC_CHECKOUT_PO option to extract GUI i18n files from git repositories.")
    endif()

    set(DIGIKAMSC_COMPILE_PO OFF)

endif()

if(DIGIKAMSC_COMPILE_DOC AND NOT EXISTS ${CMAKE_SOURCE_DIR}/doc/)

    message(STATUS "Main one documentation directory is missing. Compilation of documentation is disabled...")
    set(DIGIKAMSC_COMPILE_DOC OFF)

endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/doc-translated/)

    message(STATUS "Documentation translations directory is missing.")
    message(STATUS "You can use DIGIKAMSC_CHECKOUT_DOC option to extract doc i18n files from git repositories.")

endif()

if(DIGIKAMSC_COMPILE_PO OR DIGIKAMSC_COMPILE_DOC)
    set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)
endif()

if(BUILD_TESTING)
    include(CTest)
endif()

######################################################################################################################

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Top level compilation configuration for ${PROJECT_NAME}")
message(STATUS "")

if(DIGIKAMSC_COMPILE_DIGIKAM)
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/core)
        message(STATUS "${PROJECT_NAME} will be compiled....................... NO (source code not available)")
        set (DIGIKAMSC_COMPILE_DIGIKAM OFF)
    else()
        message(STATUS "${PROJECT_NAME} will be compiled....................... YES")
    endif()
else()
    message(STATUS     "${PROJECT_NAME} will be compiled....................... NO")
endif()

if(DIGIKAMSC_CHECKOUT_PO)
    message(STATUS "Extract application translations files......... YES")
else()
    message(STATUS "Extract application translations files......... NO")
endif()

if(DIGIKAMSC_CHECKOUT_DOC)
    message(STATUS "Extract documentation translations files....... YES")
else()
    message(STATUS "Extract documentation translations files....... NO")
endif()

if(DIGIKAMSC_COMPILE_PO)
    message(STATUS "Translations will be compiled.................. YES")
else()
    message(STATUS "Translations will be compiled.................. NO")
endif()

if(DIGIKAMSC_COMPILE_DOC)
    message(STATUS "Handbooks will be compiled..................... YES")
else()
    message(STATUS "Handbooks will be compiled..................... NO")
endif()

if(BUILD_TESTING)
    message(STATUS "Tests code will be compiled.................... YES")
else()
    message(STATUS "Tests code will be compiled.................... NO")
endif()

######################################################################################################################

if(DIGIKAMSC_COMPILE_DIGIKAM)
    add_subdirectory(core)
endif()

if(DIGIKAMSC_COMPILE_PO OR DIGIKAMSC_COMPILE_DOC)
    find_package(Gettext REQUIRED)
endif()

if(DIGIKAMSC_COMPILE_PO)
    add_subdirectory(po)
endif()

if(DIGIKAMSC_COMPILE_DOC)
    find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS DocTools)
    ECM_OPTIONAL_ADD_SUBDIRECTORY(doc)
    ECM_OPTIONAL_ADD_SUBDIRECTORY(doc-translated)
endif()
