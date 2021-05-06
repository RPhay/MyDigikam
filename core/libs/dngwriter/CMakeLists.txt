#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# DNG SDK and XMP SDK use C++ exceptions
kde_enable_exceptions()

include_directories(
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

    ${EXPAT_INCLUDE_DIR}

    ${CMAKE_CURRENT_SOURCE_DIR}/extra/md5
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFilesPlugins/api/source
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/build
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/client-glue
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/Interfaces/BaseInterfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

#------------------------------------------------------------------------------------

set(libmd5_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/extra/md5/XMP_MD5.cpp)

# Used by digikamcore
add_library(core_libmd5_obj OBJECT ${libmd5_SRCS})

target_compile_definitions(core_libmd5_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(core_libmd5_obj PRIVATE /w)
else()
    target_compile_options(core_libmd5_obj PRIVATE -w)
endif()

#------------------------------------------------------------------------------------

set(libxmp_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/WXMPFiles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/MOOV_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/PSIR_FileWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/ReconcileLegacy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/TimeConversionUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/AIFF/AIFFMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/AIFF/AIFFReconcile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/AIFF/AIFFBehavior.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/ReconcileIPTC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/ASF_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/XDCAM_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/RIFF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/ISOBaseMedia_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/TIFF_FileWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/TIFF_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/SWF_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/PSIR_MemoryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/Reconcile_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/PackageFormat_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/IPTC_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/RIFF_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/PostScript_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/ID3_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/iXMLMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/CartMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/Cr8rMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/BEXTMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/DISPMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/INFOMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/PrmLMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/WAVEReconcile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/WAVE/WAVEBehavior.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/ReconcileTIFF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/QuickTime_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/PNG_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/P2_Support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/IFF/ChunkController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/IFF/Chunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/IFF/IChunkBehavior.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/IFF/ChunkPath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/TIFF_MemoryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/XMPScanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FormatSupport/SVG_Adapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/NativeMetadataSupport/MetadataSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/NativeMetadataSupport/IReconcile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/NativeMetadataSupport/IMetadata.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/PluginManager.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/FileHandlerInstance.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/HostAPIImpl.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/XMPAtoms.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/OS_Utils_Mac.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/OS_Utils_WIN.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/Module.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/PluginHandler/OS_Utils_Linux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/XMPFiles_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/HandlerRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/MPEG2_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/XDCAMEX_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/InDesign_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/SonyHDV_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/SVG_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/ASF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/PostScript_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/AVCHD_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/Trivial_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/RIFF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/GIF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/UCF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/PNG_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/P2_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/FLV_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/PSD_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/Basic_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/TIFF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/SWF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/MP3_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/XDCAMSAM_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/MPEG4_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/XDCAM_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/AIFF_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/WAVE_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/XDCAMFAM_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/Scanner_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/FileHandlers/JPEG_Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFiles/source/XMPFiles.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFilesPlugins/api/source/HostAPIAccess.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFilesPlugins/api/source/PluginAPIImpl.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFilesPlugins/api/source/PluginBase.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFilesPlugins/api/source/PluginRegistry.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPFilesPlugins/api/source/PluginUtils.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/client-glue/TXMPUtils.incl_cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/client-glue/TXMPFiles.incl_cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/client-glue/TXMPMeta.incl_cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/client-glue/TXMPIterator.incl_cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/source/IConfigurationManager.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/source/IError.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/source/IConfigurable.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/source/IErrorNotifier.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCommon/source/IUTF8String.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMP.incl_cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IClientDOMParser.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IDOMSerializer.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IDOMParser.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IClientDOMSerializer.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IArrayNode.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IDOMImplementationRegistry.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/INodeIterator.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/INode.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/ICoreConfigurationManager.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/INameSpacePrefixMap.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IMetadata.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/ICompositeNode.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/ICoreObjectFactory.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IPathSegment.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/ISimpleNode.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IStructureNode.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/public/include/XMPCore/source/IPath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XMPFiles_IO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XMP_ProgressTracker.cpp

#    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/Host_IO-Win.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/SafeStringAPIs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/UnicodeConversions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XMP_LibUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XML_Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/XIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/UnicodeInlines.incl_cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/IOUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/PerfUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/source/Host_IO-POSIX.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta-Parse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPCore_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta-GetSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/ExpatAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/WXMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/WXMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/WXMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta-Serialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPIterator2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/ParseRDF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPUtils2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPMeta2-GetSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/source/XMPUtils-FileInfo.cpp
)

# Used by digikamcore
add_library(core_libxmp_obj OBJECT ${libxmp_SRCS})

target_compile_definitions(core_libxmp_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)
    target_compile_options(core_libxmp_obj PRIVATE /w)
else()
    target_compile_options(core_libxmp_obj PRIVATE -w)
endif()

#------------------------------------------------------------------------------------

set(libdng_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_function.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_abort_sniffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_area_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bad_pixels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_big_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bottlenecks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_camera_profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_space.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_spec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_date_time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exceptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_file_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_filter_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_fingerprint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_gain_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_globals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_hue_sat_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_ifd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image_writer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_iptc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_jpeg_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lens_correction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_linearization_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_local_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lossless_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_misc_opcodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mosaic_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_negative.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_opcode_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_opcodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_orientation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_parse_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pixel_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_preview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rational.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_read_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_ref_counted_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_reference.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_render.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_resample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_safe_arithmetic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_shared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_simple_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_spline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tag_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_temperature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tile_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tone_curve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_validate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp_sdk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xy_coord.cpp
)

# Used by digikamcore
add_library(core_libdng_obj OBJECT ${libdng_SRCS})

target_compile_definitions(core_libdng_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# Disable warnings: we will never touch this code.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)

if(MSVC)

    target_compile_options(core_libdng_obj PRIVATE /w)

else()

    target_compile_options(core_libdng_obj PRIVATE -w)

endif()

#------------------------------------------------------------------------------------

set(libdngwriter_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriter_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngwriterhost.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dngsettings.cpp
)

# Used by digikamcore
add_library(core_dngwriter_obj OBJECT ${libdngwriter_SRCS})

target_compile_definitions(core_dngwriter_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
