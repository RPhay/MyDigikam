/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-07-26
 * Description : System settings container.
 *
 * SPDX-FileCopyrightText: 2020-2023 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "systemsettings.h"

// Qt includes

#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QNetworkProxy>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

SystemSettings::SystemSettings(const QString& name)
{
    if (!name.isEmpty())
    {
        m_path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                 QDir::separator() + name + QLatin1String("_systemrc");
    }

    readSettings();
}

SystemSettings::~SystemSettings()
{
}

void SystemSettings::readSettings()
{
    if (m_path.isEmpty())
    {
        return;
    }

    QSettings settings(m_path, QSettings::IniFormat);

    settings.beginGroup(QLatin1String("System"));

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

#   ifdef Q_OS_LINUX

    useHighDpiScaling    = settings.value(QLatin1String("useHighDpiScaling"), true).toBool();
    useHighDpiPixmaps    = settings.value(QLatin1String("useHighDpiPixmaps"), true).toBool();

#   else

    useHighDpiScaling    = settings.value(QLatin1String("useHighDpiScaling"), false).toBool();
    useHighDpiPixmaps    = settings.value(QLatin1String("useHighDpiPixmaps"), false).toBool();

#   endif

#endif

    if (settings.contains(QLatin1String("disableFaceEngine")))
    {
        bool oldSetting  = settings.value(QLatin1String("disableFaceEngine"), false).toBool();
        enableFaceEngine = !oldSetting;
    }
    else
    {
        enableFaceEngine = settings.value(QLatin1String("enableFaceEngine"),  true).toBool();
    }

    enableAesthetic      = settings.value(QLatin1String("enableAesthetic"),   true).toBool();
    enableAutoTags       = settings.value(QLatin1String("enableAutoTags"),    true).toBool();
    softwareOpenGL       = settings.value(QLatin1String("softwareOpenGL"),    false).toBool();
    enableLogging        = settings.value(QLatin1String("enableLogging"),     false).toBool();
    disableOpenCL        = settings.value(QLatin1String("disableOpenCL"),     true).toBool();

    // Video Settings Windows

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    disableHWConv        = settings.value(QLatin1String("disableHWConv"),     false).toBool();
    videoBackend         = settings.value(QLatin1String("videoBackend"),      QLatin1String("ffmpeg")).toString();

#endif

    // Proxy Settings

    proxyUrl             = settings.value(QLatin1String("proxyUrl"),          QString()).toString();
    proxyPort            = settings.value(QLatin1String("proxyPort"),         8080).toInt();
    proxyUser            = settings.value(QLatin1String("proxyUser"),         QString()).toString();
    proxyPass            = settings.value(QLatin1String("proxyPass"),         QString()).toString();
    proxyType            = settings.value(QLatin1String("proxyType"),         HttpProxy).toInt();
    proxyAuth            = settings.value(QLatin1String("proxyAuth"),         false).toBool();
    settings.endGroup();

    QNetworkProxy proxy;

    // Make sure that no proxy is used for an empty string or the default value:

    if (proxyUrl.isEmpty() || (proxyUrl == QLatin1String("http://")))
    {
        proxy.setType(QNetworkProxy::NoProxy);
    }
    else
    {
        if      (proxyType == Socks5Proxy)
        {
            proxy.setType(QNetworkProxy::Socks5Proxy);
        }
        else if (proxyType == HttpProxy)
        {
            proxy.setType(QNetworkProxy::HttpProxy);
        }
        else
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Unknown proxy type! Using Http Proxy instead.";
            proxy.setType(QNetworkProxy::HttpProxy);
        }
    }

    proxy.setHostName(proxyUrl);
    proxy.setPort(proxyPort);

    if (proxyAuth)
    {
        proxy.setUser(proxyUser);
        proxy.setPassword(proxyPass);
    }

    QNetworkProxy::setApplicationProxy(proxy);
}

void SystemSettings::saveSettings()
{
    if (m_path.isEmpty())
    {
        return;
    }

    QSettings settings(m_path, QSettings::IniFormat);

    settings.beginGroup(QLatin1String("System"));

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    settings.setValue(QLatin1String("useHighDpiScaling"), useHighDpiScaling);
    settings.setValue(QLatin1String("useHighDpiPixmaps"), useHighDpiPixmaps);

#endif

    settings.setValue(QLatin1String("enableFaceEngine"),  enableFaceEngine);
    settings.setValue(QLatin1String("enableAesthetic"),   enableAesthetic);
    settings.setValue(QLatin1String("enableAutoTags"),    enableAutoTags);
    settings.setValue(QLatin1String("softwareOpenGL"),    softwareOpenGL);
    settings.setValue(QLatin1String("enableLogging"),     enableLogging);
    settings.setValue(QLatin1String("disableOpenCL"),     disableOpenCL);

    if (settings.contains(QLatin1String("disableFaceEngine")))
    {
        settings.remove(QLatin1String("disableFaceEngine"));
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    settings.setValue(QLatin1String("disableHWConv"),     disableHWConv);
    settings.setValue(QLatin1String("videoBackend"),      videoBackend);

#endif

    settings.setValue(QLatin1String("proxyUrl"),          proxyUrl);
    settings.setValue(QLatin1String("proxyPort"),         proxyPort);
    settings.setValue(QLatin1String("proxyType"),         proxyType);

    if (proxyAuth)
    {
        settings.setValue(QLatin1String("proxyAuth"),     true);
        settings.setValue(QLatin1String("proxyUser"),     proxyUser);
        settings.setValue(QLatin1String("proxyPass"),     proxyPass);
    }
    else
    {
        settings.setValue(QLatin1String("proxyAuth"),     false);
    }

    settings.endGroup();
}

} // namespace Digikam
