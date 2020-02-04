#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
# Copyright (c) 2015      by Mohamed_Anwer, <m_dot_anwer at gmx dot com>
# Copyright (C) 2018      by Mario Frank,   <mario dot frank at uni minus potsdam dot de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

### digiKam database shared library target ######################################################################################

add_library(digikamdatabase SHARED $<TARGET_OBJECTS:digikamdatabase_src>)

set_target_properties(digikamdatabase PROPERTIES VERSION ${DIGIKAM_VERSION_SHORT} SOVERSION ${DIGIKAM_VERSION_SHORT})

add_dependencies(digikamdatabase digikamcore)

# Note all this target dependencies must be private and not exported to prevent inherited external plugins dependencies.

target_link_libraries(digikamdatabase

                      PRIVATE

                      Qt5::Core
                      Qt5::Gui
                      Qt5::Sql

                      KF5::ConfigCore
                      KF5::Solid
                      KF5::I18n

                      digikamcore
)

if(ENABLE_DBUS)

    target_link_libraries(digikamdatabase
                          PRIVATE
                          Qt5::DBus
    )

endif()

### Install Rules ###############################################################################################################

install(TARGETS digikamdatabase EXPORT DigikamDatabaseConfig ${INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT DigikamDatabaseConfig DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamDatabase" NAMESPACE Digikam::)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/DigikamDatabaseConfigVersion.cmake
                                 VERSION ${DIGIKAM_VERSION_SHORT}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DigikamDatabaseConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamDatabase")
