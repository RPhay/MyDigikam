/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Item icon view interface - Image Quality Sorting.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "itemiconview_p.h"

// Local includes

#include "setupimagequalitysorter.h"
#include "maintenancethread.h"

namespace Digikam
{

void ItemIconView::slotImageQualitySorter()
{
    Setup::execSinglePage(this, Setup::ImageQualityPage);

    QStringList paths;

    for (const auto& url : selectedUrls())
    {
        paths<<url.toLocalFile();
    }

    SetupImageQualitySorter* const settingWidgets = new SetupImageQualitySorter();

    ImageQualityContainer settings                = settingWidgets->getImageQualityContainer();

    MaintenanceThread* const thread               = new MaintenanceThread(this);

    thread->sortByImageQuality(paths, settings);
    thread->start();

    connect(thread, &QThread::finished,
            thread, &QObject::deleteLater);
}

} // namespace Digikam
