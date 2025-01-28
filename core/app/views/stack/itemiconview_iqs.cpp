/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Item icon view interface - Image Quality Sorting.
 *
 * SPDX-FileCopyrightText: 2021-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021      by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    Setup::execSinglePage(qApp->activeWindow(), Setup::ImageQualityPage);

    QStringList paths;
    const auto urls = selectedUrls();

    for (const auto& url : urls)
    {
        paths << url.toLocalFile();
    }

    SetupImageQualitySorter* const settingWidgets = new SetupImageQualitySorter();

    ImageQualitySettings settings                 = settingWidgets->getImageQualitySettings();

    MaintenanceThread* const thread               = new MaintenanceThread(this);

    thread->sortByImageQuality(paths, settings);
    thread->start();

    connect(thread, &QThread::finished,
            thread, &QObject::deleteLater);
}

} // namespace Digikam
