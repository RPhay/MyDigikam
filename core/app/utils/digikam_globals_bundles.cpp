/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-08
 * Description : global macros, variables and flags - Bundle functions.
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikam_globals_p.h"

namespace Digikam
{

bool isRunningInAppImageBundle()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (env.contains(QLatin1String("APPIMAGE_ORIGINAL_LD_LIBRARY_PATH")) &&
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_QT_PLUGIN_PATH"))  &&
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_XDG_DATA_DIRS"))   &&
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_PATH")))
    {
        return true;
    }

    return false;
}

QProcessEnvironment adjustedEnvironmentForAppImage()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // If we are running into AppImage bundle, switch env var to the right values.

    if (isRunningInAppImageBundle())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Adjusting environment variables for AppImage bundle";

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_LD_LIBRARY_PATH")).isEmpty())
        {
            env.insert(QLatin1String("LD_LIBRARY_PATH"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_LD_LIBRARY_PATH")));
        }
        else
        {
            env.remove(QLatin1String("LD_LIBRARY_PATH"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_QT_PLUGIN_PATH")).isEmpty())
        {
            env.insert(QLatin1String("QT_PLUGIN_PATH"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_QT_PLUGIN_PATH")));
        }
        else
        {
            env.remove(QLatin1String("QT_PLUGIN_PATH"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_XDG_DATA_DIRS")).isEmpty())
        {
            env.insert(QLatin1String("XDG_DATA_DIRS"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_XDG_DATA_DIRS")));
        }
        else
        {
            env.remove(QLatin1String("XDG_DATA_DIRS"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_PATH")).isEmpty())
        {
            env.insert(QLatin1String("PATH"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_PATH")));
        }
        else
        {
            env.remove(QLatin1String("PATH"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_KDE_FULL_SESSION")).isEmpty())
        {
            env.insert(QLatin1String("KDE_FULL_SESSION"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_KDE_FULL_SESSION")));
        }
        else
        {
            env.remove(QLatin1String("KDE_FULL_SESSION"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_DESKTOP_SESSION")).isEmpty())
        {
            env.insert(QLatin1String("DESKTOP_SESSION"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_DESKTOP_SESSION")));
        }
        else
        {
            env.remove(QLatin1String("DESKTOP_SESSION"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_XDG_CURRENT_DESKTOP")).isEmpty())
        {
            env.insert(QLatin1String("XDG_CURRENT_DESKTOP"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_XDG_CURRENT_DESKTOP")));
        }
        else
        {
            env.remove(QLatin1String("XDG_CURRENT_DESKTOP"));
        }

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_XDG_SESSION_DESKTOP")).isEmpty())
        {
            env.insert(QLatin1String("XDG_SESSION_DESKTOP"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_XDG_SESSION_DESKTOP")));
        }
        else
        {
            env.remove(QLatin1String("XDG_SESSION_DESKTOP"));
        }
    }

    return env;
}

void tryInitDrMingw()
{

#ifdef HAVE_DRMINGW

    qCDebug(DIGIKAM_GENERAL_LOG) << "Loading DrMinGw run-time...";

    QRegExp versionRegExp(QLatin1String("(\\d+[.]*\\d*)"));
    QSysInfo::productVersion().indexOf(versionRegExp);
    double version  = versionRegExp.capturedTexts().constFirst().toDouble();

    if  (
         ((version < 2000.0) && (version < 10.0)) ||
         ((version > 2000.0) && (version < 2016.0))
        )
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DrMinGw: unsupported Windows version" << version;

        return;
    }

    QString appPath = QCoreApplication::applicationDirPath();
    QString excFile = QDir::toNativeSeparators(appPath + QLatin1String("/exchndl.dll"));

    HMODULE hModExc = LoadLibraryW((LPCWSTR)excFile.utf16());

    if (!hModExc)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DrMinGw: cannot init crash handler dll.";

        return;
    }

    // No need to call ExcHndlInit since the crash handler is installed on DllMain

    auto myExcHndlSetLogFileNameA = reinterpret_cast<BOOL (APIENTRY*)(const char*)>
                                        (GetProcAddress(hModExc, "ExcHndlSetLogFileNameA"));

    if (!myExcHndlSetLogFileNameA)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DrMinGw: cannot init customized crash file.";

        return;
    }

    // Set the log file path to %LocalAppData%\digikam_crash.log

    QString logPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    QString logFile = QDir::toNativeSeparators(logPath + QLatin1String("/digikam_crash.log"));
    myExcHndlSetLogFileNameA(logFile.toLocal8Bit().data());

    qCDebug(DIGIKAM_GENERAL_LOG) << "DrMinGw run-time loaded.";
    qCDebug(DIGIKAM_GENERAL_LOG) << "DrMinGw crash-file will be located at: " << logFile;

#endif // HAVE_DRMINGW

}

QString macOSBundlePrefix()
{
    return QString::fromUtf8("/Applications/digiKam.org/digikam.app/Contents/");
}

void unloadQtTranslationFiles(QApplication& app)
{
    // HACK: We try to remove all the translators installed by ECMQmLoader.
    // The reason is that it always load translations for the system locale
    // which interferes with our effort to handle override languages. Since
    // `en_US` (or `en`) strings are defined in code, the QTranslator doesn't
    // actually handle translations for them, so even if we try to install
    // a QTranslator loaded from `en`, the strings always get translated by
    // the system language QTranslator that ECMQmLoader installed instead
    // of the English one.

    // ECMQmLoader creates all QTranslator's parented to the active QApplication instance.

    QList<QTranslator*> translators = app.findChildren<QTranslator*>(QString(), Qt::FindDirectChildrenOnly);

    foreach (const auto& translator, translators)
    {
        app.removeTranslator(translator);
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Qt standard translations removed:" << translators.size();
}

void loadStdQtTranslationFiles(QApplication& app)
{
    QString transPath = QStandardPaths::locate(QStandardPaths::DataLocation,
                                               QLatin1String("translations"),
                                               QStandardPaths::LocateDirectory);

    if (!transPath.isEmpty())
    {
        QString languagePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                               QLatin1Char('/')                                                        +
                               QLatin1String("klanguageoverridesrc");

        qCDebug(DIGIKAM_GENERAL_LOG) << "Qt standard translations path:" << transPath;

        QLocale locale;

        if (!languagePath.isEmpty())
        {
            QSettings settings(languagePath, QSettings::IniFormat);
            settings.beginGroup(QLatin1String("Language"));
            QString language = settings.value(qApp->applicationName(), QString()).toString();
            settings.endGroup();

            if (!language.isEmpty())
            {
                locale = QLocale(language.split(QLatin1Char(':')).first());
            }
        }

        const QStringList qtCatalogs =
        {
            QLatin1String("qt"),
            QLatin1String("qtbase"),
            QLatin1String("qt_help"),

#ifdef HAVE_QWEBENGINE

            QLatin1String("qtwebengine"),
            QLatin1String("qtdeclarative"),
            QLatin1String("qtquickcontrols2"),
            QLatin1String("qtmultimedia"),

#endif

#ifdef HAVE_QTXMLPATTERNS

            QLatin1String("qtxmlpatterns"),

#endif

        };

        foreach (const QString& catalog, qtCatalogs)
        {
            QTranslator* const translator = new QTranslator(&app);

            if (translator->load(locale, catalog, QLatin1String("_"), transPath))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Loaded Qt standard translations"
                                             << locale.name()
                                             << "from catalog"
                                             << catalog;

                app.installTranslator(translator);
            }
            else
            {
                delete translator;
            }
        }
    }
}

void loadEcmQtTranslationFiles(QApplication& app)
{
    // Load translations created by the ECMPoQmTools module.
    // This function is based on the code in:
    // https://invent.kde.org/frameworks/extra-cmake-modules/-/blob/master/modules/ECMQmLoader.cpp.in

    const QStringList ecmCatalogs =
    {
        QLatin1String("kauth5_qt"),
        QLatin1String("kbookmarks5_qt"),
        QLatin1String("kcodecs5_qt"),
        QLatin1String("kcompletion5_qt"),
        QLatin1String("kconfig5_qt"),
        QLatin1String("kcoreaddons5_qt"),
        QLatin1String("kdbusaddons5_qt"),
        QLatin1String("kglobalaccel5_qt"),
        QLatin1String("kitemviews5_qt"),
        QLatin1String("kwidgetsaddons5_qt"),
        QLatin1String("kwindowsystem5_qt"),
        QLatin1String("solid5_qt"),
        QLatin1String("kde5_xml_mimetypes"),
    };

    QStringList ecmLangs = KLocalizedString::languages();
    const QString langEn = QLatin1String("en");

    // Replace "en_US" with "en" because that's what we have in the locale dir.

    int indexOfEnUs      = ecmLangs.indexOf(QLatin1String("en_US"));

    if (indexOfEnUs != -1)
    {
        ecmLangs[indexOfEnUs] = langEn;
    }

    // We need to have "en" to the end of the list, because we explicitly
    // removed the "en" translators added by ECMQmLoader.
    // If "en" is already on the list, we truncate the ones after, because
    // "en" is the catch-all fallback that has the strings in code.

    int indexOfEn = ecmLangs.indexOf(langEn);

    if (indexOfEn != -1)
    {
        for (int i = (ecmLangs.size() - indexOfEn - 1) ; i > 0 ; i--)
        {
            ecmLangs.removeLast();
        }
    }
    else
    {
        ecmLangs.append(langEn);
    }

    // The last added one has the highest precedence, so we iterate the list backwards.

    QStringListIterator it(ecmLangs);
    it.toBack();

    while (it.hasPrevious())
    {
        const QString& localeDirName = it.previous();

        foreach (const auto& catalog, ecmCatalogs)
        {
            QString subPath    = QLatin1String("locale/")       %
                                 localeDirName                  %
                                 QLatin1String("/LC_MESSAGES/") %
                                 catalog                        %
                                 QLatin1String(".qm");

            const QString root = QLibraryInfo::location(QLibraryInfo::PrefixPath);

            // For AppImage transalotion files uses AppDataLocation.

            QString fullPath   = QStandardPaths::locate(QStandardPaths::AppDataLocation, subPath);

            if (fullPath.isEmpty())
            {
                // For distro builds probably still use GenericDataLocation, so check that too.

                fullPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, subPath);
            }

            if (fullPath.isEmpty())
            {
                // And, failing all, use the deps install folder

                fullPath = root + QLatin1String("/share/") + subPath;
            }

            if (!QFile::exists(fullPath))
            {
                continue;
            }

            QTranslator* const translator = new QTranslator(&app);

            if (translator->load(fullPath))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Loaded Qt ECM translations"
                                             << localeDirName
                                             << "from catalog"
                                             << catalog;

                translator->setObjectName(QString::fromUtf8("QTranslator.%1.%2").arg(localeDirName, catalog));
                app.installTranslator(translator);
            }
            else
            {
                delete translator;
            }
        }
    }
}

void installQtTranslationFiles(QApplication& app)
{

#if defined Q_OS_WIN || defined Q_OS_MACOS

    bool installTranslations = true;

#else

    bool installTranslations = isRunningInAppImageBundle();

#endif

    if (installTranslations)
    {
        unloadQtTranslationFiles(app);
        loadStdQtTranslationFiles(app);
        loadEcmQtTranslationFiles(app);
    }
}

} // namespace Digikam
