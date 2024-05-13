/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : shared libraries list dialog common to digiKam and Showfoto
 *
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "libsinfodlg.h"
#include "digikam_config.h"

// Qt includes

#include <QStringList>
#include <QString>
#include <QTreeWidget>
#include <QHeaderView>
#include <QThread>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>

#ifdef HAVE_QWEBENGINE
#   include <QtWebEngineWidgetsVersion>
#else
#   include <QtWebKitWidgetsVersion>
#endif

// KDE includes

#include <klocalizedstring.h>
#include <kmemoryinfo.h>
#include <kxmlgui_version.h>

// Local includes

#include "digikam_debug.h"
#include "drawdecoder.h"
#include "greycstorationfilter.h"
#include "pgfutils.h"
#include "digikam-lcms.h"
#include "metaengine.h"
#include "exiftoolparser.h"
#include "loadingcache.h"
#include "itempropertiestab.h"

#ifdef HAVE_JXL
#   include "dngwriter.h"
#endif

#ifdef HAVE_LENSFUN
#   include "lensfuniface.h"
#endif

#ifdef HAVE_GEOLOCATION
#   include "backendmarble.h"
#endif

#ifdef HAVE_IMAGE_MAGICK

// Pragma directives to reduce warnings from ImageMagick header files.
#   if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wignored-qualifiers"
#       pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#   endif

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wignored-qualifiers"
#       pragma clang diagnostic ignored "-Wkeyword-macro"
#       pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#   endif

#   include <Magick++.h>

// Restore warnings

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic pop
#   endif

#   if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#       pragma GCC diagnostic pop
#   endif

#endif

// C ANSI includes

#ifndef Q_OS_WIN
extern "C"
{
#endif

#ifdef HAVE_JASPER

#   if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wcpp"
#   endif

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Werror"
#       pragma clang diagnostic ignored "-Wundef"
#   endif

#   include <jasper/jas_version.h>

// Restore warnings
#   if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#       pragma GCC diagnostic pop
#   endif

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic pop
#   endif

#endif

#include <png.h>
#include <tiffvers.h>

#ifdef HAVE_HEIF
#   include <libheif/heif_version.h>
#endif

// libx265 includes

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#   pragma clang diagnostic ignored "-Wnested-anon-types"
#   pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#ifdef HAVE_X265
#   include <x265.h>
#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

// Avoid Warnings under Win32
#undef HAVE_STDLIB_H
#undef HAVE_STDDEF_H
#include <jpeglib.h>

#ifndef Q_OS_WIN
}
#endif

#include "digikam_opencv.h"

// defined in OpenCV core/private.hpp
namespace cv
{

const char* currentParallelFramework();

}

namespace Digikam
{

LibsInfoDlg::LibsInfoDlg(QWidget* const parent)
    : InfoDlg(parent)
{
    setWindowTitle(i18nc("@title:window", "Shared Libraries and Components Information"));

    const char* const CONTEXT   = "@item: Component information, see help->components";
    const QString SUPPORTED_YES = i18nc("@item: component is supported/available",     "Yes");
    const QString SUPPORTED_NO  = i18nc("@item: component is not available/supported", "No");

    listView()->setHeaderLabels(QStringList() << QLatin1String("Properties") << QLatin1String("Value")); // Hidden header -> no i18n
    listView()->setSortingEnabled(true);
    listView()->setRootIsDecorated(true);
    listView()->setSelectionMode(QAbstractItemView::SingleSelection);
    listView()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listView()->setAllColumnsShowFocus(true);
    listView()->setColumnCount(2);
    listView()->header()->setSectionResizeMode(QHeaderView::Stretch);
    listView()->header()->hide();

    // NOTE: by default set a list of common components information used by Showfoto and digiKam.

    m_features = new QTreeWidgetItem(listView(), QStringList() << i18nc("@title", "Features"));
    listView()->addTopLevelItem(m_features);

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Parallelized demosaicing") <<        checkTriState(DRawDecoder::librawUseGomp()));

#ifdef HAVE_QTXMLPATTERNS

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Rajce support") <<                   SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Rajce support") <<                   SUPPORTED_NO);

#endif

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Exiv2 supports XMP metadata") <<     (MetaEngine::supportXmp() ?  SUPPORTED_YES : SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Exiv2 supports Base Media") <<       (MetaEngine::supportBmff() ? SUPPORTED_YES : SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Exiv2 supports JPEG-XL metadata") << (MetaEngine::supportJpegXL() ? SUPPORTED_YES : SUPPORTED_NO));

    // ---

    m_libraries = new QTreeWidgetItem(listView(), QStringList() << i18nc("@title", "Libraries"));
    listView()->addTopLevelItem(m_libraries);


    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Qt Framework") <<                   QLatin1String(qVersion()));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "KDE Framework") <<                  QLatin1String(KXMLGUI_VERSION_STRING));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibRaw") <<                         DRawDecoder::librawVersion());

#ifdef HAVE_EIGEN3

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Eigen") <<                          QLatin1String(EIGEN3_VERSION_STRING));

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Eigen support") <<                  SUPPORTED_NO);

#endif

#ifdef HAVE_QWEBENGINE

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Qt WebEngine version") <<           QLatin1String(QTWEBENGINEWIDGETS_VERSION_STR));

#else

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Qt WebKit version") <<              QLatin1String(QTWEBKITWIDGETS_VERSION_STR));

#endif

#ifdef HAVE_QMULTIMEDIA

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Qt Multimedia support") <<          SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Qt Multimedia support") <<          SUPPORTED_YES);

#endif

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Exiv2") <<                          MetaEngine::Exiv2Version());

    ExifToolParser* const parser = new ExifToolParser(this);
    ExifToolParser::ExifToolData parsed;

    if (parser->version())
    {
        parsed            = parser->currentData();
        QString etVersion = parsed.find(QLatin1String("VERSION_STRING")).value()[0].toString();
        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc(CONTEXT, "ExifTool") <<                   etVersion);
    }
    else
    {
        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc(CONTEXT, "ExifTool support") <<           SUPPORTED_NO);
    }

#ifdef HAVE_LENSFUN

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LensFun") <<                        LensFunIface::lensFunVersion());

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "LensFun support") <<                SUPPORTED_NO);

#endif

#ifdef HAVE_IMAGE_MAGICK

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "ImageMagick codecs") <<             QLatin1String(MagickLibVersionText));

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "ImageMagick codecs support") <<     SUPPORTED_NO);

#endif

#ifdef HAVE_HEIF

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibHEIF") <<                        QLatin1String(LIBHEIF_VERSION));
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "HEIF reading support") <<           SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "HEIF reading support") <<           SUPPORTED_NO);

#endif

#ifdef HAVE_X265

    const x265_api* const x265api = x265_api_get(0);

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Libx265") <<             (x265api ? QLatin1String(x265api->version_str)
                                                                          : i18nc("@info: version", "Unknown")));
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "HEIF writing support") <<           SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "HEIF Writing support") <<           SUPPORTED_NO);

#endif

    QString tiffver = QLatin1String(TIFFLIB_VERSION_STR);
    tiffver         = tiffver.left(tiffver.indexOf(QLatin1Char('\n')));
    tiffver         = tiffver.section(QLatin1Char(' '), 2, 2);
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibTIFF") <<                        tiffver);

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibPNG") <<                         QLatin1String(PNG_LIBPNG_VER_STRING));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibJPEG") <<                        QString::number(JPEG_LIB_VERSION));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibCImg") <<                        GreycstorationFilter::cimgVersionString());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibLCMS") <<                        QString::number(LCMS_VERSION));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibPGF") <<                         PGFUtils::libPGFVersion());

#ifdef HAVE_JXL

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "XMP SDK") <<                        DNGWriter::xmpSdkVersion());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "DNG SDK") <<                        DNGWriter::dngSdkVersion());

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "DNG support") <<                    SUPPORTED_NO);

#endif

#ifdef HAVE_JASPER

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibJasper") <<                      QLatin1String(jas_getversion()));

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "LibJasper support") <<              SUPPORTED_NO);

#endif

#ifdef HAVE_GEOLOCATION

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Geolocation support") <<            SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Geolocation support") <<            SUPPORTED_NO);

#endif

#ifdef HAVE_SONNET

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Spell-Checking support") <<         SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Spell-Checking support") <<         SUPPORTED_NO);

#endif

    int nbcore         = QThread::idealThreadCount();
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18ncp(CONTEXT, "CPU core", "CPU cores", nbcore) << QString::fromLatin1("%1").arg(nbcore));

    KMemoryInfo memInfo;

    if (!memInfo.isNull())
    {
        quint64 available = memInfo.totalPhysical();

        if (available > 0)
        {
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc(CONTEXT, "Memory available") << ItemPropertiesTab::humanReadableBytesCount(available));
        }
        else
        {
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc(CONTEXT, "Memory available") << i18nc("@item: information about memory", "Unknown"));
        }
    }

    quint64 cacheSize = LoadingCache::cache()->getCacheSize();

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Image cache size") << ItemPropertiesTab::humanReadableBytesCount(cacheSize));

    // NOTE: MANIFEST.txt is a text file generated with the bundles and listing all git revisions of rolling release components.
    //       One section title start with '+'.
    //       All component revisions are listed below line by line with the name and the revision separated by ':'.
    //       More than one section can be listed in manifest.

    const QString gitRevs = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                   QLatin1String("MANIFEST.txt"));

    if (!gitRevs.isEmpty() && QFile::exists(gitRevs))
    {
        QFile file(gitRevs);

        if (!file.open(QIODevice::ReadOnly))
        {
            return;
        }

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Git revisions manifest file found:" << gitRevs;

        QTreeWidgetItem* const manifestHead = new QTreeWidgetItem(listView(), QStringList() << i18nc("@item: component info", "Manifests"));
        listView()->addTopLevelItem(manifestHead);

        QTextStream in(&file);
        QTreeWidgetItem* manifestEntry = nullptr;

#ifndef __clang_analyzer__

        // Disable false-positive memory leak reported by scan-build with QTreeWidgetItem.

        while (!in.atEnd())
        {
            QString line = in.readLine();

            if (line.isEmpty())
            {
                continue;
            }

            if      (line.startsWith(QLatin1Char('+')))
            {
                manifestEntry = new QTreeWidgetItem(manifestHead, QStringList() << line.remove(QLatin1Char('+')));
            }
            else if (manifestEntry)
            {
                new QTreeWidgetItem(manifestEntry, line.split(QLatin1Char(':')));
            }
        }

#endif

        file.close();
    }

    QTreeWidgetItem* const opencvHead = new QTreeWidgetItem(listView(), QStringList() << i18nc("@item: opencv info", "OpenCV Configuration"));
    listView()->addTopLevelItem(opencvHead);

    // --- OpenCV::OpenCL features

    try
    {
        if (!cv::ocl::haveOpenCL() || !cv::ocl::useOpenCL())
        {
            new QTreeWidgetItem(opencvHead, QStringList() <<
                                i18nc(CONTEXT, "OpenCL availability") << SUPPORTED_NO);
        }
        else
        {
            std::vector<cv::ocl::PlatformInfo> platforms;
            cv::ocl::getPlatfomsInfo(platforms);

            if (platforms.empty())
            {
                new QTreeWidgetItem(opencvHead, QStringList() <<
                                    i18nc(CONTEXT, "OpenCL availability") << SUPPORTED_NO);

            }
            else
            {
                QTreeWidgetItem* const oclplfrm = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "OpenCL platform"));

                for (size_t i = 0 ; i < platforms.size() ; i++)
                {
                    const cv::ocl::PlatformInfo* platform = &platforms[i];

                    QTreeWidgetItem* const plfrm = new QTreeWidgetItem(oclplfrm, QStringList() << QString::fromStdString(platform->name()));

                    cv::ocl::Device current_device;

                    for (int j = 0 ; j < platform->deviceNumber() ; j++)
                    {
                        platform->getDevice(current_device, j);
                        QString deviceTypeStr = openCVGetDeviceTypeString(current_device);

                        new QTreeWidgetItem(plfrm, QStringList()
                            << deviceTypeStr << QString::fromStdString(current_device.name()) +
                               QLatin1String(" (") + QString::fromStdString(current_device.version()) + QLatin1Char(')'));
                    }

                    if (plfrm->childCount() == 0)
                    {
                        new QTreeWidgetItem(plfrm, QStringList() << i18n("no entry"));
                    }
                }

                if (oclplfrm->childCount() == 0)
                {
                    new QTreeWidgetItem(oclplfrm, QStringList() << i18n("no entry"));
                }

                const cv::ocl::Device& device = cv::ocl::Device::getDefault();

                if (!device.available())
                {
                    new QTreeWidgetItem(opencvHead, QStringList() <<
                                        i18nc(CONTEXT, "OpenCL device") << SUPPORTED_NO);
                }
                else
                {
                    QTreeWidgetItem* const ocldev = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "OpenCL Device"));
                    QString deviceTypeStr         = openCVGetDeviceTypeString(device);

                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Type")                       << deviceTypeStr);
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Name")                       << QString::fromStdString(device.name()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Version")                    << QString::fromStdString(device.version()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Driver version")             << QString::fromStdString(device.driverVersion()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Address bits")               << QString::number(device.addressBits()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Compute units")              << QString::number(device.maxComputeUnits()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Max work group size")        << QString::number(device.maxWorkGroupSize()));

                    QString localMemorySizeStr = openCVBytesToStringRepr(device.localMemSize());
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Local memory size")          << localMemorySizeStr);

                    QString maxMemAllocSizeStr = openCVBytesToStringRepr(device.maxMemAllocSize());
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Max memory allocation size") << maxMemAllocSizeStr);

                    QString doubleSupportStr = (device.doubleFPConfig() > 0) ? SUPPORTED_YES : SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Double support")             << doubleSupportStr);

                    QString halfSupportStr = (device.halfFPConfig() > 0) ? SUPPORTED_YES : SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Half support")               << halfSupportStr);

                    QString isUnifiedMemoryStr = device.hostUnifiedMemory() ? SUPPORTED_YES : SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Host unified memory")        << isUnifiedMemoryStr);

                    QString haveAmdBlasStr = cv::ocl::haveAmdBlas() ? SUPPORTED_YES : SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Has AMD Blas")                  << haveAmdBlasStr);

                    QString haveAmdFftStr  = cv::ocl::haveAmdFft() ? SUPPORTED_YES : SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Has AMD Fft")                   << haveAmdFftStr);
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width char")   << QString::number(device.preferredVectorWidthChar()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width short")  << QString::number(device.preferredVectorWidthShort()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width int")    << QString::number(device.preferredVectorWidthInt()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width long")   << QString::number(device.preferredVectorWidthLong()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width float")  << QString::number(device.preferredVectorWidthFloat()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width double") << QString::number(device.preferredVectorWidthDouble()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(CONTEXT, "Preferred vector width half")   << QString::number(device.preferredVectorWidthHalf()));

                    QTreeWidgetItem* const ocldevext = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "OpenCL Device extensions"));
                    QString extensionsStr            = QString::fromStdString(device.extensions());
                    int pos                          = 0;

                    while (pos < extensionsStr.size())
                    {
                        int pos2 = extensionsStr.indexOf(QLatin1Char(' '), pos);

                        if (pos2 == -1)
                        {
                            pos2 = extensionsStr.size();
                        }

                        if (pos2 > pos)
                        {
                            QString extensionName = extensionsStr.mid(pos, pos2 - pos);
                            new QTreeWidgetItem(ocldevext, QStringList() << extensionName << SUPPORTED_YES);
                        }

                        pos = pos2 + 1;
                    }

                    if (ocldevext->childCount() == 0)
                    {
                        new QTreeWidgetItem(ocldevext, QStringList() << i18n("no entry"));
                    }
                }
            }
        }
    }
    catch (...)
    {
        new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "OpenCL availability") << SUPPORTED_NO);
    }

    // --- OpenCV::Hardware features

    try
    {
        QTreeWidgetItem* const ocvhdw = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "Hardware features"));
        int count                     = 0;

        for (int i = 0 ; i < CV_HARDWARE_MAX_FEATURE ; i++)
        {
            QString name = QString::fromStdString(cv::getHardwareFeatureName(i));

            if (name.isEmpty())
            {
                continue;
            }

            bool enabled = cv::checkHardwareSupport(i);

            if (enabled)
            {
                count++;
                new QTreeWidgetItem(ocvhdw, QStringList() << name << SUPPORTED_YES);
            }
        }
    }
    catch (...)
    {
        new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "Hardware features availability") << SUPPORTED_NO);
    }

    // --- OpenCV::Threads features

    try
    {
        QTreeWidgetItem* const ocvthreads = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "Threads features"));

        QString parallelFramework = QString::fromStdString(cv::currentParallelFramework());

        if (!parallelFramework.isEmpty())
        {
            new QTreeWidgetItem(ocvthreads, QStringList() << i18nc(CONTEXT, "Number of Threads") << QString::number(cv::getNumThreads()));
            new QTreeWidgetItem(ocvthreads, QStringList() << i18nc(CONTEXT, "Parallel framework") << parallelFramework);
        }
    }
    catch (...)
    {
        new QTreeWidgetItem(opencvHead, QStringList() << i18nc(CONTEXT, "Threads features availability") << SUPPORTED_NO);
    }
}

QString LibsInfoDlg::checkTriState(int value) const
{
    switch (value)
    {
        case true:
        {
            return i18nc("@info: tri state", "Yes");
        }

        case false:
        {
            return i18nc("@info: tri state", "No");
        }

        default:
        {
            return i18nc("@info: tri state", "Unknown");
        }
    }
}

QString LibsInfoDlg::openCVBytesToStringRepr(size_t value) const
{
    size_t b = value % 1024;
    value /= 1024;

    size_t kb = value % 1024;
    value /= 1024;

    size_t mb = value % 1024;
    value /= 1024;

    size_t gb = value;

    QString s;
    QTextStream stream(&s);

    if (gb > 0)
    {
        stream << gb << " GB ";
    }

    if (mb > 0)
    {
        stream << mb << " MB ";
    }

    if (kb > 0)
    {
        stream << kb << " KB ";
    }

    if (b > 0)
    {
        stream << b << " B";
    }

    if (s[s.size() - 1] == QLatin1Char(' '))
    {
        s = s.mid(0, s.size() - 1);
    }

    return s;
}

QString LibsInfoDlg::openCVGetDeviceTypeString(const cv::ocl::Device& device)
{
    if (device.type() == cv::ocl::Device::TYPE_CPU)
    {
        return QLatin1String("CPU");
    }

    if (device.type() == cv::ocl::Device::TYPE_GPU)
    {
        if (device.hostUnifiedMemory())
        {
            return QLatin1String("iGPU");
        }
        else
        {
            return QLatin1String("dGPU");
        }
    }

    return QLatin1String("unknown");
}

} // namespace Digikam

#include "moc_libsinfodlg.cpp"
