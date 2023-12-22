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

#ifndef DIGIKAM_SYSTEM_SETTINGS_H
#define DIGIKAM_SYSTEM_SETTINGS_H

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
        HttpProxy,          ///< Uses an Http proxy
        Socks5Proxy         ///< Uses a Socks5Proxy
    };

public:

    explicit SystemSettings(const QString& name);
    ~SystemSettings();

public:

    void saveSettings();

public:

    bool useHighDpiScaling;
    bool useHighDpiPixmaps;
    bool enableFaceEngine;
    bool enableAesthetic;
    bool enableAutoTags;
    bool enableLogging;
    bool disableOpenCL;

    // Proxy Settings.

    QString proxyUrl;
    int     proxyPort;
    QString proxyUser;
    QString proxyPass;
    bool    proxyType;
    bool    proxyAuth;

private:

    void readSettings();

private:

    QString m_path;
};

} // namespace Digikam

#endif // DIGIKAM_SYSTEM_SETTINGS_H
