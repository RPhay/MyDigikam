/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : shared libraries list dialog common to digiKam and Showfoto
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "libsinfodlg_p.h"

namespace Digikam
{

LibsInfoDlg::LibsInfoDlg(QWidget* const parent)
    : InfoDlg(parent),
      d      (new Private)
{
    setWindowTitle(i18nc("@title:window", "Shared Libraries and Components Information"));

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
                        i18nc(Private::CONTEXT, "Parallelized demosaicing") <<        d->checkTriState(DRawDecoder::librawUseGomp()));

#ifdef HAVE_QTXMLPATTERNS

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Rajce support") <<                   Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Rajce support") <<                   Private::SUPPORTED_NO);

#endif

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2 supports XMP metadata") <<     (MetaEngine::supportXmp() ?  Private::SUPPORTED_YES : Private::SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2 supports Base Media") <<       (MetaEngine::supportBmff() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2 supports JPEG-XL metadata") << (MetaEngine::supportJpegXL() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO));

    // ---

    m_libraries = new QTreeWidgetItem(listView(), QStringList() << i18nc("@title", "Libraries"));
    listView()->addTopLevelItem(m_libraries);


    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "Qt Framework") <<                   QLatin1String(qVersion()));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "KDE Framework") <<                  QLatin1String(KXMLGUI_VERSION_STRING));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibRaw") <<                         DRawDecoder::librawVersion());

#ifdef HAVE_EIGEN3

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "Eigen") <<                          QLatin1String(EIGEN3_VERSION_STRING));

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Eigen support") <<                  Private::SUPPORTED_NO);

#endif

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "Qt WebEngine version") <<           QLatin1String(QTWEBENGINEWIDGETS_VERSION_STR));

#ifdef HAVE_QMULTIMEDIA

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Qt Multimedia support") <<          Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Qt Multimedia support") <<          Private::SUPPORTED_YES);

#endif

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2") <<                          MetaEngine::Exiv2Version());

    ExifToolParser* const parser = new ExifToolParser(this);
    ExifToolParser::ExifToolData parsed;

    if (parser->version())
    {
        parsed            = parser->currentData();
        QString etVersion = parsed.find(QLatin1String("VERSION_STRING")).value()[0].toString();
        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc(Private::CONTEXT, "ExifTool") <<                   etVersion);
    }
    else
    {
        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc(Private::CONTEXT, "ExifTool support") <<           Private::SUPPORTED_NO);
    }

#ifdef HAVE_LENSFUN

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LensFun") <<                        LensFunIface::lensFunVersion());

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "LensFun support") <<                Private::SUPPORTED_NO);

#endif

#ifdef HAVE_IMAGE_MAGICK

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "ImageMagick codecs") <<             QLatin1String(MagickLibVersionText));

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "ImageMagick codecs support") <<     Private::SUPPORTED_NO);

#endif

#ifdef HAVE_HEIF

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibHEIF") <<                        QLatin1String(LIBHEIF_VERSION));
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "HEIF reading support") <<           Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "HEIF reading support") <<           Private::SUPPORTED_NO);

#endif

#ifdef HAVE_X265

    const x265_api* const x265api = x265_api_get(0);

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "Libx265") <<             (x265api ? QLatin1String(x265api->version_str)
                                                                          : i18nc("@info: libx265 version", "Unknown")));
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "HEIF writing support") <<           Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "HEIF Writing support") <<           Private::SUPPORTED_NO);

#endif

    QString tiffver = QLatin1String(TIFFLIB_VERSION_STR);
    tiffver         = tiffver.left(tiffver.indexOf(QLatin1Char('\n')));
    tiffver         = tiffver.section(QLatin1Char(' '), 2, 2);
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibTIFF") <<                        tiffver);

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibPNG") <<                         QLatin1String(PNG_LIBPNG_VER_STRING));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibJPEG") <<                        QString::number(JPEG_LIB_VERSION));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibCImg") <<                        GreycstorationFilter::cimgVersionString());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibLCMS") <<                        QString::number(LCMS_VERSION));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibPGF") <<                         PGFUtils::libPGFVersion());

#ifdef HAVE_JXL

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "XMP SDK") <<                        DNGWriter::xmpSdkVersion());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "DNG SDK") <<                        DNGWriter::dngSdkVersion());

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "DNG support") <<                    Private::SUPPORTED_NO);

#endif

#ifdef HAVE_JASPER

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "LibJasper") <<                      QLatin1String(jas_getversion()));

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "LibJasper support") <<              Private::SUPPORTED_NO);

#endif

#ifdef HAVE_GEOLOCATION

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(Private::CONTEXT, "Geolocation support") <<            Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Geolocation support") <<            Private::SUPPORTED_NO);

#endif

#ifdef HAVE_SONNET

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Spell-Checking support") <<         Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Spell-Checking support") <<         Private::SUPPORTED_NO);

#endif

    int nbcore         = QThread::idealThreadCount();
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18ncp(Private::CONTEXT, "CPU core", "CPU cores", nbcore) << QString::fromLatin1("%1").arg(nbcore));

    m_buildtools       = new QTreeWidgetItem(listView(), QStringList() << i18nc("@title", "Build Environment"));
    listView()->addTopLevelItem(m_buildtools);

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Host")                   <<         QLatin1String(CMAKE_HOST_PRETTY_NAME));

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Cmake")                  <<         QLatin1String(CMAKE_VERSION_STRING));

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Compiler")               <<         QString::fromLatin1("%1 - %2").arg(QLatin1String(CMAKE_CXX_COMPILER_ID))
                                                                                                          .arg(QLatin1String(CMAKE_CXX_COMPILER_VERSION)));
#ifdef CCACHE_VERSION

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Ccache")                 <<         QLatin1String(CCACHE_VERSION));

#else

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Ccache support")         <<         Private::SUPPORTED_NO);

#endif

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Build date")             <<         QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat));

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Build target")           <<         QLatin1String(digikam_build_type));

    new QTreeWidgetItem(m_buildtools, QStringList() <<
                        i18nc(Private::CONTEXT, "Build architecture")     <<         QSysInfo::buildCpuArchitecture());

    QString gitRev     = QLatin1String(GITVERSION);
    QString gitBra     = QLatin1String(GITBRANCH);

    if (
        !gitRev.isEmpty()                           &&
        !gitBra.isEmpty()                           &&
        !gitRev.startsWith(QLatin1String("unknow")) &&
        !gitRev.startsWith(QLatin1String("export")) &&
        !gitBra.startsWith(QLatin1String("unknow"))
       )
    {
        const int maxStringLength         = 10;
        QString gitVer                    = gitRev.left(maxStringLength / 2 - 2) +
                                            QLatin1String("...")                 +
                                            gitRev.right(maxStringLength / 2 - 1);

        QLabel* const gitRevLbl           = new QLabel(QString::fromLatin1("<a href='https://invent.kde.org/graphics/digikam/commit/%1'>%2</a>")
                                                       .arg(gitRev).arg(gitVer),
                                                       listView());
        gitRevLbl->setOpenExternalLinks(true);

        QTreeWidgetItem* const gitRevItem = new QTreeWidgetItem(m_buildtools);
        gitRevItem->setText(0, i18nc(Private::CONTEXT, "Git revision"));
        listView()->setItemWidget(gitRevItem, 1, gitRevLbl);

        new QTreeWidgetItem(m_buildtools, QStringList()    <<
                            i18nc(Private::CONTEXT, "Git branch")   << gitBra);
    }
    else
    {
        new QTreeWidgetItem(m_buildtools, QStringList()    <<
                            i18nc(Private::CONTEXT, "Git revision") << i18nc("@info: git revision", "Unknown"));

        new QTreeWidgetItem(m_buildtools, QStringList()    <<
                            i18nc(Private::CONTEXT, "Git branch")   << i18nc("@info: git branch", "Unknown"));
    }

    KMemoryInfo memInfo;

    if (!memInfo.isNull())
    {
        quint64 available = memInfo.totalPhysical();

        if (available > 0)
        {
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc(Private::CONTEXT, "Memory available") << ItemPropertiesTab::humanReadableBytesCount(available));
        }
        else
        {
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc(Private::CONTEXT, "Memory available") << i18nc("@item: information about memory", "Unknown"));
        }
    }

    quint64 cacheSize = LoadingCache::cache()->getCacheSize();

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Image cache size") << ItemPropertiesTab::humanReadableBytesCount(cacheSize));

    /**
     * NOTE: MANIFEST.txt is a text file generated with the bundles and listing all git revisions of rolling release components.
     *       One section title start with '+'.
     *       All component revisions are listed below line by line with the name and the revision separated by ':'.
     *       More than one section can be listed in manifest.
     */

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

    // --- OpenCV::OpenCL features.

    try
    {
        if (!cv::ocl::haveOpenCL() || !cv::ocl::useOpenCL())
        {
            new QTreeWidgetItem(opencvHead, QStringList() <<
                                i18nc(Private::CONTEXT, "OpenCL availability") << Private::SUPPORTED_NO);
        }
        else
        {
            std::vector<cv::ocl::PlatformInfo> platforms;
            cv::ocl::getPlatfomsInfo(platforms);

            if (platforms.empty())
            {
                new QTreeWidgetItem(opencvHead, QStringList() <<
                                    i18nc(Private::CONTEXT, "OpenCL availability") << Private::SUPPORTED_NO);

            }
            else
            {
                QTreeWidgetItem* const oclplfrm = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "OpenCL platform"));

                for (size_t i = 0 ; i < platforms.size() ; i++)
                {
                    const cv::ocl::PlatformInfo* platform = &platforms[i];

                    QTreeWidgetItem* const plfrm = new QTreeWidgetItem(oclplfrm, QStringList() << QString::fromStdString(platform->name()));

                    cv::ocl::Device current_device;

                    for (int j = 0 ; j < platform->deviceNumber() ; j++)
                    {
                        platform->getDevice(current_device, j);
                        QString deviceTypeStr = d->openCVGetDeviceTypeString(current_device);

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
                                        i18nc(Private::CONTEXT, "OpenCL device") << Private::SUPPORTED_NO);
                }
                else
                {
                    QTreeWidgetItem* const ocldev = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "OpenCL Device"));
                    QString deviceTypeStr         = d->openCVGetDeviceTypeString(device);

                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Type")                       << deviceTypeStr);
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Name")                       << QString::fromStdString(device.name()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Version")                    << QString::fromStdString(device.version()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Driver version")             << QString::fromStdString(device.driverVersion()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Address bits")               << QString::number(device.addressBits()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Compute units")              << QString::number(device.maxComputeUnits()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Max work group size")        << QString::number(device.maxWorkGroupSize()));

                    QString localMemorySizeStr = d->openCVBytesToStringRepr(device.localMemSize());
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Local memory size")          << localMemorySizeStr);

                    QString maxMemAllocSizeStr = d->openCVBytesToStringRepr(device.maxMemAllocSize());
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Max memory allocation size") << maxMemAllocSizeStr);

                    QString doubleSupportStr = (device.doubleFPConfig() > 0) ? Private::SUPPORTED_YES : Private::SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Double support")             << doubleSupportStr);

                    QString halfSupportStr = (device.halfFPConfig() > 0) ? Private::SUPPORTED_YES : Private::SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Half support")               << halfSupportStr);

                    QString isUnifiedMemoryStr = device.hostUnifiedMemory() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Host unified memory")        << isUnifiedMemoryStr);

                    QString haveAmdBlasStr = cv::ocl::haveAmdBlas() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Has AMD Blas")                  << haveAmdBlasStr);

                    QString haveAmdFftStr  = cv::ocl::haveAmdFft() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO;
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Has AMD Fft")                   << haveAmdFftStr);
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width char")   << QString::number(device.preferredVectorWidthChar()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width short")  << QString::number(device.preferredVectorWidthShort()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width int")    << QString::number(device.preferredVectorWidthInt()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width long")   << QString::number(device.preferredVectorWidthLong()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width float")  << QString::number(device.preferredVectorWidthFloat()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width double") << QString::number(device.preferredVectorWidthDouble()));
                    new QTreeWidgetItem(ocldev, QStringList() << i18nc(Private::CONTEXT, "Preferred vector width half")   << QString::number(device.preferredVectorWidthHalf()));

                    QTreeWidgetItem* const ocldevext = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "OpenCL Device extensions"));
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
                            new QTreeWidgetItem(ocldevext, QStringList() << extensionName << Private::SUPPORTED_YES);
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
        new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "OpenCL availability") << Private::SUPPORTED_NO);
    }

    // --- OpenCV::Hardware features.

    try
    {
        QTreeWidgetItem* const ocvhdw = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "Hardware features"));
        int count                     = 0;
        Q_UNUSED(count);

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
                new QTreeWidgetItem(ocvhdw, QStringList() << name << Private::SUPPORTED_YES);
            }
        }
    }
    catch (...)
    {
        new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "Hardware features availability") << Private::SUPPORTED_NO);
    }

    // --- OpenCV::Threads features.

    try
    {
        QTreeWidgetItem* const ocvthreads = new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "Threads features"));

        QString parallelFramework = QString::fromStdString(cv::currentParallelFramework());

        if (!parallelFramework.isEmpty())
        {
            new QTreeWidgetItem(ocvthreads, QStringList() << i18nc(Private::CONTEXT, "Number of Threads") << QString::number(cv::getNumThreads()));
            new QTreeWidgetItem(ocvthreads, QStringList() << i18nc(Private::CONTEXT, "Parallel framework") << parallelFramework);
        }
    }
    catch (...)
    {
        new QTreeWidgetItem(opencvHead, QStringList() << i18nc(Private::CONTEXT, "Threads features availability") << Private::SUPPORTED_NO);
    }
}

LibsInfoDlg::~LibsInfoDlg()
{
    delete d;
}

} // namespace Digikam

#include "moc_libsinfodlg.cpp"
