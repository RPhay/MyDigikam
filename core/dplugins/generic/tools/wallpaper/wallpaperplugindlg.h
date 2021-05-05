/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper - Settings dialog
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_WALLPAPER_PLUGIN_DLG_H
#define DIGIKAM_WALLPAPER_PLUGIN_DLG_H

// Local includes

#include "dplugin.h"
#include "dplugindialog.h"

using namespace Digikam;

namespace DigikamGenericWallpaperPlugin
{

class WallpaperPluginDlg : public DPluginDialog
{
    Q_OBJECT

public:

    explicit WallpaperPluginDlg(DPlugin* const plugin,
                                QWidget* const parent = nullptr);
    ~WallpaperPluginDlg() override;

    int wallpaperLayout() const;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericWallpaperPlugin

#endif // DIGIKAM_WALLPAPER_PLUGIN_DLG_H
