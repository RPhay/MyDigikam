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

#pragma once

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SystemSettings
{
public:

    /**
     * @brief This enum is used to specify the proxy that is used.
     */
    enum ProxyType
    {
        HttpProxy   = 0,    ///< Uses an Http proxy.
        Socks5Proxy         ///< Uses a Socks5 proxy.
    };

public:

    explicit SystemSettings(const QString& name);
    ~SystemSettings();

public:

    void saveSettings();

public:

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    bool useHighDpiScaling = false;
    bool useHighDpiPixmaps = false;

#endif

    bool enableFaceEngine  = false;
    bool enableAesthetic   = false;
    bool enableAutoTags    = false;
    bool softwareOpenGL    = false;
    bool enableLogging     = false;
    bool disableOpenCL     = true;

    // Video Settings Windows

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    bool    disableHWConv  = false;
    QString videoBackend   = QLatin1String("ffmpeg");

#endif

    // Proxy Settings.

    QString proxyUrl;
    int     proxyPort      = 8080;
    QString proxyUser;
    QString proxyPass;
    int     proxyType      = HttpProxy;
    bool    proxyAuth      = false;

private:

    void readSettings();

private:

    QString m_path;
};

} // namespace Digikam
