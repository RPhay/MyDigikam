#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# SPDX-FileCopyrightText: 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# digiKam core object library
#

set(DIGIKAMCORE_OBJECTS

            ${CMAKE_CURRENT_SOURCE_DIR}/utils/digikam_debug.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/utils/digikam_version.cpp

            $<TARGET_OBJECTS:core_dimg_obj>
            $<TARGET_OBJECTS:core_imgqsort_obj>
            $<TARGET_OBJECTS:core_dragdrop_obj>
            $<TARGET_OBJECTS:core_dmetadata_obj>
            $<TARGET_OBJECTS:core_onlineversion_obj>
            $<TARGET_OBJECTS:core_jpegutils_obj>
            $<TARGET_OBJECTS:core_progressmanager_obj>
            $<TARGET_OBJECTS:core_threadimageio_obj>
            $<TARGET_OBJECTS:core_pgfutils_obj>
            $<TARGET_OBJECTS:core_dthread_obj>
            $<TARGET_OBJECTS:core_networkmanager_obj>
            $<TARGET_OBJECTS:core_versionmanager_obj>
            $<TARGET_OBJECTS:core_libraw_obj>
            $<TARGET_OBJECTS:core_rawengine_obj>
            $<TARGET_OBJECTS:core_dpluginsinterface_obj>
            $<TARGET_OBJECTS:core_libwso2_obj>

            $<TARGET_OBJECTS:core_libmd5_obj>
            $<TARGET_OBJECTS:core_libxmp_obj>
            $<TARGET_OBJECTS:core_libdng_obj>
            $<TARGET_OBJECTS:core_dngwriter_obj>

            # widgets
            $<TARGET_OBJECTS:core_digikamwidgets_obj>
            $<TARGET_OBJECTS:core_digikamdialogs_obj>
            $<TARGET_OBJECTS:core_itemproperties_obj>
            $<TARGET_OBJECTS:core_digikamgenericmodels_obj>
            $<TARGET_OBJECTS:core_notificationmanager_obj>

            # utilities
            $<TARGET_OBJECTS:core_setupcommon_obj>
            $<TARGET_OBJECTS:core_imageeditor_obj>
            $<TARGET_OBJECTS:core_libtransitionmngr_obj>
            $<TARGET_OBJECTS:core_timeadjust_obj>

            $<TARGET_OBJECTS:core_digikamdatabase_obj>
            $<TARGET_OBJECTS:core_digikamfacesengine_obj>
            $<TARGET_OBJECTS:core_videotoolscommon_obj>
)

if(ENABLE_MEDIAPLAYER)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:core_videotools_obj>
        $<TARGET_OBJECTS:core_qtav_obj>
    )

endif()

if(Marble_FOUND)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:core_geoiface_obj>
        $<TARGET_OBJECTS:core_geomapwrapper_obj>
    )

endif()

if(KF5FileMetaData_FOUND)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:core_baloowrap_obj>
    )

endif()

if(KF5AkonadiContact_FOUND)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:core_akonadiiface_obj>
    )

endif()

add_library(digikamcore
            SHARED
            ${DIGIKAMCORE_OBJECTS}
)

set_target_properties(digikamcore PROPERTIES
                      VERSION ${DIGIKAM_VERSION_SHORT}
                      SOVERSION ${DIGIKAM_VERSION_SHORT}
)

target_compile_definitions(digikamcore
                           PRIVATE
                           digikamcore_EXPORTS
)

target_include_directories(digikamcore
                           PRIVATE
                           ${DIGIKAM_TARGET_INCLUDES}
)

add_dependencies(digikamcore digikam-gitversion)
add_dependencies(digikamcore digikam-builddate)

# All codes from this target are exported with digikam_core_export.h header and DIGIKAM_EXPORT macro.
generate_export_header(digikamcore
                       BASE_NAME digikam
                       EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/utils/digikam_core_export.h"
)

# NOTE: all this target dependencies must be private and not exported
# to prevent inherited dependencies on external plugins.

target_link_libraries(digikamcore

                      PRIVATE

                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Xml
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::Sql
                      Qt${QT_VERSION_MAJOR}::PrintSupport
                      Qt${QT_VERSION_MAJOR}::Concurrent

                      KF5::Solid
                      KF5::WindowSystem
                      KF5::ConfigGui
                      KF5::XmlGui
                      KF5::I18n
                      KF5::Service

                      # Required by CImg which use pthread internally.

                      ${CMAKE_THREAD_LIBS_INIT}
                      ${EXPAT_LIBRARY}

                      ${LCMS2_LIBRARIES} # filters

                      ${TIFF_LIBRARIES}
                      PNG::PNG
                      ${JPEG_LIBRARIES}
                      exiv2lib

                      ${OPENMP_LDFLAGS}

                      opencv_core
                      opencv_objdetect
                      opencv_imgproc
                      opencv_imgcodecs
                      opencv_dnn
                      opencv_ml
                      opencv_flann
)

if(Qt6_FOUND)
    target_link_libraries(digikamcore
                          PRIVATE
                          Qt${QT_VERSION_MAJOR}::StateMachine
    )
endif()

if(ENABLE_QWEBENGINE)

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    )

else()

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt${QT_VERSION_MAJOR}::WebKitWidgets
    )

endif()

if(ENABLE_DBUS)

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt${QT_VERSION_MAJOR}::DBus
    )

endif()

if(ENABLE_MEDIAPLAYER)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${MEDIAPLAYER_LIBRARIES}
    )

endif()

if(KF5IconThemes_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::IconThemes
    )

endif()

if(KF5KIO_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::KIOCore
                          KF5::KIOWidgets
    )

endif()

if(KF5Notifications_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::Notifications
    )

endif()

if(KF5NotifyConfig_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::NotifyConfig
    )

endif()

if(KF5Sonnet_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::SonnetCore
                          KF5::SonnetUi
    )

endif()

if(Marble_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${MARBLE_LIBRARIES}
    )

endif()

if(X11_FOUND)

    if(Qt6_FOUND)

        target_link_libraries(digikamcore
                              PRIVATE
                              ${X11_LIBRARIES}
        )

    else()


        target_link_libraries(digikamcore
                              PRIVATE
                              Qt5::X11Extras
                              ${X11_LIBRARIES}
        )

    endif()

endif()

if(Jasper_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${JASPER_LIBRARIES}
    )

endif()

# LibLqr-1 library rules for content-aware filter
if(LQR-1_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${LQR-1_LIBRARIES}
    )

endif()

# For HEIF file format support
if(HEIF_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          Libheif::Libheif
    )

endif()

if(X265_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${X265_LIBRARIES}
    )

endif()

if(LensFun_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${LENSFUN_LIBRARIES}
    )

endif()

if(ImageMagick_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${ImageMagick_LIBRARIES}
    )

endif()

# for nrfilter
if(OpenCV_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${OpenCV_LIBRARIES}
    )

endif()

if(KF5FileMetaData_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::FileMetaData
    )

endif()

if(KF5AkonadiContact_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::AkonadiContact
    )

endif()

if(APPLE)

    target_link_libraries(digikamcore
                          PRIVATE
                          "-framework AppKit"
    )

endif()

if(WIN32)

    target_link_libraries(digikamcore
                          PRIVATE
                          # Defined in RulesWindows.cmake
                          ${WSOCK32_LIBRARY}
                          ${WS2_32_LIBRARY}
                          ${NETAPI32_LIBRARY}
                          ${USEENV_LIBRARY}
                          ${PSAPI_LIBRARY}
    )

endif()

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${KVM_LIBRARY}
    )

endif()

### Install Rules ###############################################################################################################

install(TARGETS digikamcore EXPORT DigikamCoreConfig ${INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT DigikamCoreConfig DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamCore" NAMESPACE Digikam::)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/DigikamCoreConfigVersion.cmake
                                 VERSION ${DIGIKAM_VERSION_SHORT}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DigikamCoreConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamCore")

if(APPLE)
    install(FILES "$<TARGET_FILE:digikamcore>.dSYM" DESTINATION "${CMAKE_INSTALL_LIBDIR}" CONFIGURATIONS Debug RelWithDebInfo)
endif()
