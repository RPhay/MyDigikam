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
      disableOpenCL    (true),
      m_appName        (name)
{
}

SystemSettings::~SystemSettings()
{
}

void SystemSettings::readSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                   QLatin1Char('/') + m_appName + QLatin1String("_systemrc");

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup(QLatin1String("System"));
    useHighDpiScaling = settings.value(QLatin1String("useHighDpiScaling"), false).toBool();
    useHighDpiPixmaps = settings.value(QLatin1String("useHighDpiPixmaps"), false).toBool();
    disableOpenCL     = settings.value(QLatin1String("disableOpenCL"),     true).toBool();
    settings.endGroup();
}

void SystemSettings::saveSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                   QLatin1Char('/') + m_appName + QLatin1String("_systemrc");

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup(QLatin1String("System"));
    settings.setValue(QLatin1String("useHighDpiScaling"), useHighDpiScaling);
    settings.setValue(QLatin1String("useHighDpiPixmaps"), useHighDpiPixmaps);
    settings.setValue(QLatin1String("disableOpenCL"),     disableOpenCL);
    settings.endGroup();
}

} // namespace Digikam
