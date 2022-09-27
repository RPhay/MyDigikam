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

#include "systemsettings.h"

// Qt includes

#include <QSettings>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

SystemSettings::SystemSettings(const QString& name)
    : useHighDpiScaling(false),
      useHighDpiPixmaps(false),
      disableFaceEngine(false),
      enableLogging    (false),
      disableOpenCL    (true)
{
    if (!name.isEmpty())
    {
        m_path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                 QLatin1Char('/') + name + QLatin1String("_systemrc");
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
    useHighDpiScaling = settings.value(QLatin1String("useHighDpiScaling"), false).toBool();
    useHighDpiPixmaps = settings.value(QLatin1String("useHighDpiPixmaps"), false).toBool();
    disableFaceEngine = settings.value(QLatin1String("disableFaceEngine"), false).toBool();
    enableLogging     = settings.value(QLatin1String("enableLogging"),     false).toBool();
    disableOpenCL     = settings.value(QLatin1String("disableOpenCL"),     true).toBool();
    settings.endGroup();
}

void SystemSettings::saveSettings()
{
    if (m_path.isEmpty())
    {
        return;
    }

    QSettings settings(m_path, QSettings::IniFormat);

    settings.beginGroup(QLatin1String("System"));
    settings.setValue(QLatin1String("useHighDpiScaling"), useHighDpiScaling);
    settings.setValue(QLatin1String("useHighDpiPixmaps"), useHighDpiPixmaps);
    settings.setValue(QLatin1String("disableFaceEngine"), disableFaceEngine);
    settings.setValue(QLatin1String("enableLogging"),     enableLogging);
    settings.setValue(QLatin1String("disableOpenCL"),     disableOpenCL);
    settings.endGroup();
}

} // namespace Digikam
