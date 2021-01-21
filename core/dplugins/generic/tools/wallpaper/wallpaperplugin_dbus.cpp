/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper - DBUS version.
 * References  : https://bugs.kde.org/show_bug.cgi?id=217950
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

#include <QMessageBox>
#include <QProcess>
#include <QDBusMessage>
#include <QDBusConnection>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericWallpaperPlugin
{

bool WallpaperPlugin::setWallpaper(const QString& path) const
{
    QDBusMessage message = QDBusMessage::createMethodCall
    (
        QLatin1String("org.kde.plasmashell"),
        QLatin1String("/PlasmaShell"),
        QLatin1String("org.kde.PlasmaShell"),
        QLatin1String("evaluateScript")
    );

    /**
     * Exemple of WallPaper settings from plasma-org.kde.plasma.desktop-appletsrc:
     *
     * [Containments][1][Wallpaper][org.kde.image][General]
     * FillMode=2
     * Image=file:///home/gilles/Images/M104_left.jpg
     * height=1440
     * width=2560
     *
     * FillMode can take these values:
     *    0 = Adjusted
     *    1 = Adjusted with apect ratio
     *    2 = Adjusted and cropped
     *    3 = Mosaic
     *    4 = ???
     *    5 = ???
     *    6 = Centered
     */

    message << QString::fromUtf8
    (
        "var allDesktops = desktops();"
        "for (i=0;i<allDesktops.length;i++)"
        "{"
            "d = allDesktops[i];"
            "d.wallpaperPlugin = \"org.kde.image\";"
            "d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");"
            "d.writeConfig(\"Image\", \"%1\")"
        "}"
    )
    .arg(path);

    QDBusMessage reply = QDBusConnection::sessionBus().call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        QMessageBox::warning(nullptr,
                             i18nc("@title:window",
                                   "Error while to set image as wallpaper"),
                             i18n("Cannot change wallpaper image from current desktop with\n"
                                  "%1\n\nError: %2",
                                  path,
                                  reply.errorMessage()));

        return false;
    }

    return true;
}

} // namespace DigikamGenericWallpaperPlugin
