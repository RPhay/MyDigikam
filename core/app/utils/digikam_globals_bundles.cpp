/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-08
 * Description : global macros, variables and flags - Bundle functions.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "digikam_globals_p.h"

namespace Digikam
{

bool isRunningInAppImageBundle()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_LD_LIBRARY_PATH")) &&
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_QT_PLUGIN_PATH"))  &&
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_XDG_DATA_DIRS"))   &&
        env.contains(QLatin1String("APPIMAGE_ORIGINAL_PATH"))
       )
    {
        return true;
    }

    return false;
}

bool isRunningOnNativeKDE()
{

#ifdef Q_OS_LINUX

    const QByteArray xdgCurrentDesktop = qgetenv("XDG_CURRENT_DESKTOP");

    return (!isRunningInAppImageBundle() && (xdgCurrentDesktop.toUpper() == "KDE"));

#else

    return false;

#endif

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

        if (!env.value(QLatin1String("APPIMAGE_ORIGINAL_LD_PRELOAD")).isEmpty())
        {
            env.insert(QLatin1String("LD_PRELOAD"),
                       env.value(QLatin1String("APPIMAGE_ORIGINAL_LD_PRELOAD")));
        }
        else
        {
            env.remove(QLatin1String("LD_PRELOAD"));
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

    Q_FOREACH (const auto& translator, translators)
    {
        app.removeTranslator(translator);
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Qt standard translations removed:" << translators.size();
}

void loadStdQtTranslationFiles(QApplication& app)
{
    QString transPath = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation,
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
                QString languageName = language.split(QLatin1Char(':')).first();

                qCDebug(DIGIKAM_GENERAL_LOG) << "Language set to:" << languageName;

                locale = QLocale(languageName);
            }
        }

        const QStringList qtCatalogs =
        {
            QLatin1String("qt"),
            QLatin1String("qtbase"),
            QLatin1String("qt_help"),
            QLatin1String("qtdeclarative"),
            QLatin1String("qtquickcontrols"),
            QLatin1String("qtquickcontrols2"),
            QLatin1String("qtmultimedia"),
            QLatin1String("qtwebengine"),

#ifdef HAVE_QTXMLPATTERNS

            QLatin1String("qtxmlpatterns"),

#endif

        };

        Q_FOREACH (const QString& catalog, qtCatalogs)
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    const QStringList ecmCatalogs =
    {
//        QLatin1String("kauth6_qt"),               Do not exists.
        QLatin1String("kbookmarks6_qt"),
        QLatin1String("kcodecs6_qt"),
        QLatin1String("kcompletion6_qt"),
        QLatin1String("kconfig6_qt"),
        QLatin1String("kcoreaddons6_qt"),
        QLatin1String("kdbusaddons6_qt"),
//        QLatin1String("kde6_xml_mimetypes"),      Do not exists.
//        QLatin1String("kglobalaccel6_qt"),        Do not exists.
        QLatin1String("kitemviews6_qt"),
        QLatin1String("kwidgetsaddons6_qt"),
        QLatin1String("kwindowsystem6_qt"),
        QLatin1String("solid6_qt"),

#else

    const QStringList ecmCatalogs =
    {
        QLatin1String("kauth5_qt"),
        QLatin1String("kbookmarks5_qt"),
        QLatin1String("kcodecs5_qt"),
        QLatin1String("kcompletion5_qt"),
        QLatin1String("kconfig5_qt"),
        QLatin1String("kcoreaddons5_qt"),
        QLatin1String("kdbusaddons5_qt"),
        QLatin1String("kde5_xml_mimetypes"),
        QLatin1String("kglobalaccel5_qt"),
        QLatin1String("kitemviews5_qt"),
        QLatin1String("kwidgetsaddons5_qt"),
        QLatin1String("kwindowsystem5_qt"),
        QLatin1String("solid5_qt"),

#endif

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

        Q_FOREACH (const auto& catalog, ecmCatalogs)
        {
            QString subPath    = QLatin1String("locale/")       +
                                 localeDirName                  +
                                 QLatin1String("/LC_MESSAGES/") +
                                 catalog                        +
                                 QLatin1String(".qm");

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

            const QString root = QLibraryInfo::path(QLibraryInfo::PrefixPath);

#else

            const QString root = QLibraryInfo::location(QLibraryInfo::PrefixPath);

#endif

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
