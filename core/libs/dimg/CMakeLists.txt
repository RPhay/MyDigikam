#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

# Boost uses operator names (and, not, ...)
string(REPLACE "-fno-operator-names" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(libdimg_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_bitsops.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_colors.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_copy.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_data.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_fileio.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_metadata.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_props.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_qimage.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_qpixmap.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_scale.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/dimg_transform.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/dimg/color/dcolor.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/color/dcolorcomposer.cpp

    ${CMAKE_SOURCE_DIR}/core/libs/dimg/history/dimagehistory.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/history/filteraction.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/history/historyimageid.cpp
)

set(libdimgfilters_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgbuiltinfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgthreadedfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgthreadedanalyser.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgfiltermanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dimgfiltergenerator.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/dpixelsaliasfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/filteractionfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/randomnumbergenerator.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/raw/rawprocessingfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/raw/drawdecoding.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/decorate/borderfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/decorate/bordersettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/decorate/texturefilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/film/filmfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/blurfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/blurfxfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/colorfxfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/colorfxsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/distortionfxfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/charcoalfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/embossfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/filmgrainfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/filmgrainsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/invertfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/oilpaintfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/fx/raindropfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/auto/autolevelsfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/auto/autoexpofilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/auto/equalizefilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/auto/stretchfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/auto/normalizefilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/cb/cbfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/cb/cbsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bcg/bcgfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bcg/bcgsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bw/bwsepiafilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bw/bwsepiasettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bw/tonalityfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bw/infraredfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bw/mixerfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/bw/mixersettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/hsl/hslfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/hsl/hslsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/hsl/hspreviewwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/iccmanager.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/iccprofile.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/iccprofilesettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/icctransform.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/icctransformfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/iccsettingscontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/iccsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/icc/digikam-lcms.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lc/localcontrastfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lc/localcontrastsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lc/localcontrastcontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/nr/nrfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/nr/nrestimate.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/nr/nrsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/sharp/sharpenfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/sharp/unsharpmaskfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/sharp/sharpsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/levels/imagelevels.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/levels/levelsfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/levels/imagehistogram.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/levels/histogrambox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/levels/histogramwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/levels/histogrampainter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/curvescontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/imagecurves.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/curvesfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/curvessettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/curveswidget.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/curves/curvesbox.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/wb/wbcontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/wb/wbfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/wb/wbsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/transform/freerotationfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/transform/freerotationsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/transform/shearfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/transform/autocrop.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/greycstoration/greycstorationfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/greycstoration/greycstorationsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/antivignettingfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/antivignettingsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/lensdistortionfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/lensdistortionpixelaccess.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/redeye/redeyecorrectionfilter.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/redeye/redeyecorrectionsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/redeye/redeyecorrectioncontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalitycontainer.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalitysettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalityparser.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalityparser_blur.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalityparser_exposure.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalityparser_noise.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/imgqsort/imagequalityparser_compression.cpp
)

# ==================================================================================================
# get the gcc version

# CImg.h version 1.2.8 do not compile fine with gcc 4.3.x
# See bug #163118: digikam-0.9.4_beta5 compilation hangs with gcc 4.3
# Using -fno-tree-pre is work around this problem.

# TODO is this hack anymore required?
if(CMAKE_COMPILER_IS_GNUCXX)
    exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

    if (${GCC_VERSION} VERSION_LESS "4.3.0")
        message(STATUS "Adjusting compilation flags for GCC version (${GCC_VERSION} )")
        add_definitions(-fno-tree-pre)
    endif()
endif()


if(Lqr-1_FOUND)
    set(libdimgfilters_SRCS
        ${libdimgfilters_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/transform/contentawarefilter.cpp
    )
    include_directories(${LQR-1_INCLUDE_DIRS})
endif()

if(LensFun_FOUND)
    set(libdimgfilters_SRCS
        ${libdimgfilters_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/lensfunfilter.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/lensfuncameraselector.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/lensfuniface.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/lens/lensfunsettings.cpp
    )
    include_directories(${LENSFUN_INCLUDE_DIRS})
endif()

if(Eigen3_FOUND)
    set(libdimgfilters_SRCS
        ${libdimgfilters_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/sharp/refocusfilter.cpp
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/filters/sharp/refocusmatrix.cpp
    )
    include_directories(${EIGEN3_INCLUDE_DIR})
endif()

set(libdimgloaders_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/dimgloader.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/jpegsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/pngsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/tiffsettings.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/pgfsettings.cpp
)

# JPEG2000 support

if(Jasper_FOUND)
    set(libdimgloaders_SRCS
        ${libdimgloaders_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/jp2ksettings.cpp
    )
endif()

# HEIF support
if(X265_FOUND)
    set(libdimgloaders_SRCS
        ${libdimgloaders_SRCS}
        ${CMAKE_SOURCE_DIR}/core/libs/dimg/loaders/heifsettings.cpp
    )
endif()

include_directories(
    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
)

if(X11_FOUND)
    include_directories($<TARGET_PROPERTY:Qt5::X11Extras,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

add_library(dimg_src OBJECT
            ${libdimgloaders_SRCS}
            ${libdimgfilters_SRCS}
            ${libdimg_SRCS}
)
