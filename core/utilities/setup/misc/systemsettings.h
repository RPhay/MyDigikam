/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-07-26
 * Description : System settings container.
 *
 * SPDX-FileCopyrightText: 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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

    explicit SystemSettings(const QString& name);
    ~SystemSettings();

public:

    void saveSettings();

public:

    bool useHighDpiScaling;

    bool useHighDpiPixmaps;

    bool disableFaceEngine;

    bool enableLogging;

    bool disableOpenCL;

private:

    void readSettings();

private:

    QString m_path;
};

} // namespace Digikam

#endif // DIGIKAM_SYSTEM_SETTINGS_H
