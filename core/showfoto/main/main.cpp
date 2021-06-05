/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : showFoto is a stand alone version of image
 *               editor with no support of digiKam database.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "digikam_config.h"

// Qt includes

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QCommandLineOption>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kaboutdata.h>

// ImageMagick includes

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wkeyword-macro"
#endif

#ifdef HAVE_IMAGE_MAGICK
#   include <Magick++.h>
using namespace Magick;
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

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

using namespace Digikam;

int main(int argc, char* argv[])
{
    SystemSettings system(QLatin1String("showfoto"));
    system.readSettings();

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

#ifdef HAVE_QWEBENGINE

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#endif

    QApplication app(argc, argv);

    digikamSetDebugFilterRules();

    tryInitDrMingw();

#ifdef HAVE_IMAGE_MAGICK

    InitializeMagick(nullptr);

#endif

    // if we have some local breeze icon resource, prefer it

    DXmlGuiWindow::setupIconTheme();

    KLocalizedString::setApplicationDomain("digikam");

    KAboutData aboutData(QLatin1String("showfoto"), // component name
                         i18n("Showfoto"),          // display name
                         digiKamVersion());         // NOTE: showFoto version = digiKam version

    aboutData.setShortDescription(QString::fromUtf8("%1 - %2").arg(DAboutData::digiKamSlogan()).arg(DAboutData::digiKamFamily()));
    aboutData.setLicense(KAboutLicense::GPL);
    aboutData.setCopyrightStatement(DAboutData::copyright());
    aboutData.setOtherText(additionalInformation());
    aboutData.setHomepage(DAboutData::webProjectUrl().url());
    aboutData.setProductName(QByteArray("digikam/showfoto"));   // For bugzilla
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                            i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    DAboutData::authorsRegistration(aboutData);

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QLatin1String("files"), i18n("File(s) or folder(s) to open"), QLatin1String("[file(s) or folder(s)]"));
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("ImageViewer Settings"));
    QString iconTheme         = group.readEntry(QLatin1String("Icon Theme"), QString());

#if defined Q_OS_WIN || defined Q_OS_MACOS

    bool loadTranslation = true;

#else

    bool loadTranslation = isRunningInAppImageBundle();

#endif

    QString transPath = QStandardPaths::locate(QStandardPaths::DataLocation,
                                               QLatin1String("translations"),
                                               QStandardPaths::LocateDirectory);

    QTranslator translator;

    if (loadTranslation && !transPath.isEmpty())
    {
        QString klanguagePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                                QLatin1Char('/') + QLatin1String("klanguageoverridesrc");

        if (!klanguagePath.isEmpty())
        {
            QSettings settings(klanguagePath, QSettings::IniFormat);
            settings.beginGroup(QLatin1String("Language"));
            QString language = settings.value(qApp->applicationName(), QString()).toString();
            settings.endGroup();

            if (!language.isEmpty())
            {
                QLocale::setDefault(language.split(QLatin1Char(':')).first());
            }
        }

        bool ret = translator.load(QLocale(), QLatin1String("qtbase"),
                                   QLatin1String("_"), transPath);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Qt translations path:" << transPath;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Locale:"  << QLocale().name()
                                     << "Loading:" << ret;

        if (ret)
        {
            app.installTranslator(&translator);
        }
    }

    MetaEngine::initializeExiv2();

    // Force to use application icon for non plasma desktop as Unity for ex.

    QApplication::setWindowIcon(QIcon::fromTheme(QLatin1String("showfoto"), app.windowIcon()));

#ifdef Q_OS_WIN

    if (QSysInfo::currentCpuArchitecture().contains(QLatin1String("64")) &&
        !QSysInfo::buildCpuArchitecture().contains(QLatin1String("64")))
    {
        QMessageBox::critical(qApp->activeWindow(),
                              qApp->applicationName(),
                              i18n("<p>You are running Showfoto as a 32-bit version on a 64-bit Windows.</p>"
                                   "<p>Please install the 64-bit version of Showfoto to get "
                                   "a better experience with Showfoto.</p>"));
    }

#endif

    QList<QUrl> urlList;
    QStringList urls = parser.positionalArguments();

    foreach (const QString& url, urls)
    {
        urlList.append(QUrl::fromLocalFile(url));
    }

    parser.clearPositionalArguments();

    if (!iconTheme.isEmpty())
    {
        QIcon::setThemeName(iconTheme);
    }

    // Workaround for the automatic icon theme color
    // in KF-5.80, depending on the color scheme.

    if      (QIcon::themeName() == QLatin1String("breeze-dark"))
    {
        qApp->setPalette(QPalette(Qt::darkGray));
    }
    else if (QIcon::themeName() == QLatin1String("breeze"))
    {
        qApp->setPalette(QPalette(Qt::white));
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

    QPointer<FilesDownloader> floader = new FilesDownloader(qApp->activeWindow());

    if (!floader->checkDownloadFiles())
    {
        floader->startDownload();
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
