/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-07-26
 * Description : System settings container.
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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

    void readSettings();
    void saveSettings();

public:

    bool useHighDpiScaling;

    bool useHighDpiPixmaps;

    bool disableOpenCL;

private:

    QString m_appName;
};

} // namespace Digikam

#endif // DIGIKAM_SYSTEM_SETTINGS_H
