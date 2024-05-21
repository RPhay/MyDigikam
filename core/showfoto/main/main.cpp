/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : showFoto is a stand alone version of image
 *               editor with no support of digiKam database.
 *
 * SPDX-FileCopyrightText: 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "digikam_config.h"

// Qt includes

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QImageReader>
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QCommandLineOption>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kmemoryinfo.h>
#include <kaboutdata.h>

// ImageMagick includes


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
#   endif

#   include <Magick++.h>

using namespace Magick;

// Restore warnings

#   if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#       pragma GCC diagnostic pop
#   endif

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic pop
#   endif

#endif // HAVE_IMAGE_MAGICK

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "digikam_version.h"
#include "filesdownloader.h"
#include "systemsettings.h"
#include "metaengine.h"
#include "daboutdata.h"
#include "showfoto.h"

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#   include <objbase.h>
#endif

#if defined Q_OS_WIN
#   define MAIN_EXPORT __declspec(dllexport)
#   define MAIN_FN showfoto_main
#else
#   define MAIN_EXPORT
#   define MAIN_FN main
#endif

using namespace Digikam;

extern "C" MAIN_EXPORT int MAIN_FN(int argc, char** argv)
{
    SystemSettings system(QLatin1String("showfoto"));

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    // These settings has no effect with Qt6 (always enabled)

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps,
                                   system.useHighDpiPixmaps);

    if (system.useHighDpiScaling)
    {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
    else
    {
        QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    }

#else

    KMemoryInfo memInfo;
    quint64 maxLimit = memInfo.totalPhysical() / 1024 / 1024 / 1.40;
    maxLimit         = qMax((quint64)1024, maxLimit);
    QImageReader::setAllocationLimit(maxLimit);

#endif

    if (system.softwareOpenGL)
    {
        QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    }

    // OpenCV crash with face engine with OpenCL support
    // https://bugs.kde.org/show_bug.cgi?id=423632
    // https://bugs.kde.org/show_bug.cgi?id=426175

    if (system.disableOpenCL)
    {
        qputenv("OPENCV_OPENCL_RUNTIME", "disabled");
        qputenv("OPENCV_OPENCL_DEVICE",  "disabled");
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    if (system.disableHWConv)
    {
        qputenv("QT_DISABLE_HW_TEXTURES_CONVERSION", "1");
    }

    qputenv("QT_MEDIA_BACKEND", system.videoBackend.toLatin1());

#endif

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor);

#ifdef HAVE_QWEBENGINE

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#endif

    QApplication app(argc, argv);

    digikamSetDebugFilterRules(system.enableLogging);

    tryInitDrMingw();

#ifdef HAVE_IMAGE_MAGICK

#ifdef Q_CC_MSVC

    qputenv("MAGICK_CODER_MODULE_PATH", qApp->applicationDirPath().toUtf8());
    qputenv("MAGICK_CODER_FILTER_PATH", qApp->applicationDirPath().toUtf8());

#endif

    InitializeMagick(nullptr);

#endif

#ifdef Q_OS_MACOS

    // See bug #461734

    app.setAttribute(Qt::AA_DontShowIconsInMenus, true);

#endif

    // if we have some local breeze icon resource, prefer it

    DXmlGuiWindow::setupIconTheme();

    KLocalizedString::setApplicationDomain("digikam");

    KAboutData aboutData(QLatin1String("showfoto"),     // component name
                         i18nc("@title", "Showfoto"),   // display name
                         digiKamVersion());             // NOTE: showFoto version = digiKam version

    aboutData.setShortDescription(QString::fromUtf8("%1 - %2").arg(DAboutData::digiKamSlogan()).arg(DAboutData::digiKamFamily()));
    aboutData.setLicense(KAboutLicense::GPL);
    aboutData.setCopyrightStatement(DAboutData::copyright());
    aboutData.setOtherText(additionalInformation());
    aboutData.setHomepage(DAboutData::webProjectUrl().url());
    aboutData.setProductName(QByteArray("digikam/showfoto-core"));   // For bugzilla
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                            i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    DAboutData::authorsRegistration(aboutData);

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QLatin1String("files"),
                                 i18nc("command line option", "File(s) or folder(s) to open"),
                                 QLatin1String("[file(s) or folder(s)]"));
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("ImageViewer Settings"));
    QString iconTheme         = group.readEntry(QLatin1String("Icon Theme"), QString());
    QString colorTheme        = group.readEntry(QLatin1String("Theme"), QString::fromLatin1("Standard"));

    // See bug #438701

    installQtTranslationFiles(app);

    // ---

    MetaEngine::initializeExiv2();

    // Force to use application icon for non plasma desktop as Unity for ex.

    QApplication::setWindowIcon(QIcon::fromTheme(QLatin1String("showfoto"), app.windowIcon()));

#ifdef Q_OS_WIN

    if (
        QSysInfo::currentCpuArchitecture().contains(QLatin1String("64")) &&
        !QSysInfo::buildCpuArchitecture().contains(QLatin1String("64"))
       )
    {
        QMessageBox::critical(qApp->activeWindow(),
                              qApp->applicationName(),
                              i18nc("#info", "<p>You are running Showfoto as a 32-bit version on a 64-bit Windows.</p>"
                                    "<p>Please install the 64-bit version of Showfoto to get "
                                    "a better experience with Showfoto.</p>"));
    }

#endif

    QList<QUrl> urlList;
    QStringList urls = parser.positionalArguments();

    Q_FOREACH (const QString& url, urls)
    {
        urlList.append(QUrl::fromLocalFile(url));
    }

    parser.clearPositionalArguments();

    if (!iconTheme.isEmpty())
    {
        QIcon::setThemeName(iconTheme);
    }

#ifdef Q_OS_WIN

    // Necessary to open native open with dialog on windows

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

#endif

    ShowFoto::Showfoto* const w = new ShowFoto::Showfoto(urlList);

    // If application storage place in home directory to save customized XML settings files do not exist, create it,
    // else QFile will not able to create new files as well.

    if (!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)))
    {
        QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    // If application cache place in home directory to save cached files do not exist, create it.

    if (!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)))
    {
        QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

    w->show();

    if (system.enableFaceEngine || system.enableAesthetic || system.enableAutoTags)
    {
        QPointer<FilesDownloader> floader = new FilesDownloader(qApp->activeWindow());

        if (!floader->checkDownloadFiles())
        {
            floader->startDownload();
        }

        delete floader;
    }

    int ret = app.exec();

#ifdef Q_OS_WIN

    // Necessary to open native open with dialog on windows

    CoUninitialize();

#endif

#ifdef HAVE_IMAGE_MAGICK
#   if MagickLibVersion >= 0x693

    TerminateMagick();

#   endif
#endif

    return ret;
}
