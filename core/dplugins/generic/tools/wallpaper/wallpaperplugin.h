/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper
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

#ifndef DIGIKAM_WALLPAPER_PLUGIN_H
#define DIGIKAM_WALLPAPER_PLUGIN_H

// Qt includes

#include <QString>
#include <QIcon>
#include <QList>

// Local includes

#include "dplugingeneric.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.generic.Wallpaper"

using namespace Digikam;

namespace DigikamGenericWallpaperPlugin
{

class WallpaperPlugin : public DPluginGeneric
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginGeneric)

public:

    /**
     * Default values for Plasma Wallpaper plugin.
     */
    enum WallpaperLayout
    {
        Adjusted            = 0,
        AdjustedAspectRatio = 1,
        AdjustedCropped     = 2,
        Mosaic              = 3,
        Centered            = 6
    };

public:

    explicit WallpaperPlugin(QObject* const parent = nullptr);
    ~WallpaperPlugin()                   override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

private Q_SLOTS:

    void slotWallpaper();

private:

    bool setWallpaper(const QString& path, int layout = AdjustedCropped) const;
};

} // namespace DigikamGenericWallpaperPlugin

#endif // DIGIKAM_WALLPAPER_PLUGIN_H
