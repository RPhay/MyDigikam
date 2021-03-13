/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper - macOS version.
 *
 * Copyright (C) 2019      by Igor Antropov <antropovi at yahoo dot com>
 * Copyright (C) 2019-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "wallpaperplugin.h"

// Qt includes

#include <QStringList>
#include <QMessageBox>
#include <QProcess>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericWallpaperPlugin
{

bool WallpaperPlugin::setWallpaper(const QString& path, int layout) const
{
    /**
     * NOTE: sound like macOS < BigSur do not provide a way to customize the wallpaper layout.
     * Find a way to pass layout setting to osascript.
     * Possible values are in this order from macOS control panel: Adjusted, AdjustedAspectRatio, AdjustedCropped, Centered, Mosaic.
     */
    Q_UNUSED(layout);

    QStringList args;
    args << QLatin1String("-e");
    args << QLatin1String("tell application \"System Events\"");
    args << QLatin1String("-e");
    args << QLatin1String("tell current desktop");
    args << QLatin1String("-e");
    args << QString::fromUtf8("set picture to POSIX file \"%1\"").arg(path);
    args << QLatin1String("-e");
    args << QLatin1String("end tell");
    args << QLatin1String("-e");
    args << QLatin1String("end tell");
    args << QLatin1String("-e");
    args << QLatin1String("return");

    int ret = QProcess::execute(QLatin1String("/usr/bin/osascript"), args);

    if ((ret == -1) || (ret == 2))
    {
        QMessageBox::warning(nullptr,
                             i18nc("@title: window",
                                   "Error while to set image as wallpaper"),
                             i18nc("@info", "Cannot change wallpaper image from current desktop with\n%1",
                                   path));

        return false;
    }

    return true;
}

} // namespace DigikamGenericWallpaperPlugin
