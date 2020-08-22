/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-07
 * Description : Hot Pixels Fixer batch tool.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_HOT_PIXELS_H
#define DIGIKAM_BQM_HOT_PIXELS_H

// Local includes

#include "batchtool.h"
#include "hotpixelsettings.h"

using namespace Digikam;

namespace DigikamBqmHotPixelsPlugin
{

class HotPixels : public BatchTool
{
    Q_OBJECT

public:

    explicit HotPixels(QObject* const parent = nullptr);
    ~HotPixels();

    BatchToolSettings defaultSettings();

    BatchTool* clone(QObject* const parent=nullptr) const { return new HotPixels(parent); };

    void registerSettingsWidget();

private:

    bool toolOperations();

private Q_SLOTS:

    void slotAssignSettings2Widget();
    void slotSettingsChanged();

private:

    HotPixelSettings* m_settingsView;
};

} // namespace DigikamBqmHotPixelsPlugin

#endif // DIGIKAM_BQM_HOT_PIXELS_H
