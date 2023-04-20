#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED
                                    COMPONENTS
                                    XmlGui
                                    CoreAddons
                                    Config
                                    Service
                                    WindowSystem
                                    Solid
                                    I18n
)

find_package(KF5 ${KF5_MIN_VERSION} QUIET
                                    OPTIONAL_COMPONENTS
                                    KIO                         # For Desktop integration (Widgets only).
                                    IconThemes                  # For Desktop integration.
                                    ThreadWeaver                # For Panorama tool.
                                    NotifyConfig                # Plasma desktop application notify configuration.
                                    Notifications               # Plasma desktop notifications integration.
                                    Sonnet                      # For spell-checking.
)

if(ENABLE_KFILEMETADATASUPPORT)

    find_package(KF5 ${KF5_MIN_VERSION} QUIET
                                        OPTIONAL_COMPONENTS
                                        FileMetaData            # For Plasma destop file indexer support.
    )

endif()


if(ENABLE_AKONADICONTACTSUPPORT)

    find_package(KF5 ${AKONADI_MIN_VERSION} QUIET
                                            OPTIONAL_COMPONENTS
                                            Akonadi
                                            AkonadiContact      # For KDE Mail Contacts support.
                                            Contacts            # API for contacts/address book data.
    )

endif()

find_package(KF5 ${KSANE_MIN_VERSION} QUIET
                                      OPTIONAL_COMPONENTS
                                      Sane                      # For digital scanner support.
)

find_package(KF5 ${CALENDAR_MIN_VERSION} QUIET
                                         OPTIONAL_COMPONENTS
                                         CalendarCore           # For Calendar tool.
)

if ("${KF5CalendarCore_VERSION}" VERSION_GREATER 5.6.40)

    set(HAVE_KCALENDAR_QDATETIME TRUE)

endif()

if(ENABLE_AKONADICONTACTSUPPORT AND (NOT KF5AkonadiContact_FOUND OR NOT KF5Contacts_FOUND))

    set(ENABLE_AKONADICONTACTSUPPORT OFF)

endif()

if(ENABLE_KFILEMETADATASUPPORT AND NOT KF5FileMetaData_FOUND)

    set(ENABLE_KFILEMETADATASUPPORT OFF)

endif()

# Check if KIO have been compiled with KIOWidgets. digiKam only needs this one.

if(KF5KIO_FOUND)

    get_target_property(KIOWidgets_INCLUDE_DIRS KF5::KIOWidgets
                        INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "KF5::KIOWidgets include dirs: ${KIOWidgets_INCLUDE_DIRS}")

    if(NOT KIOWidgets_INCLUDE_DIRS)

        message(STATUS "KF5::KIOWidgets not available in shared KIO library. KIO support disabled.")
        set(KF5KIO_FOUND FALSE)

    endif()

endif()
