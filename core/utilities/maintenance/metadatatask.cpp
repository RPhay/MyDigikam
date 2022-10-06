/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-09
 * Description : Thread actions task for metadata synchronizer.
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "metadatatask.h"

// Local includes

#include "collectionscanner.h"
#include "scancontroller.h"
#include "metadatahub.h"
#include "digikam_debug.h"
#include "maintenancedata.h"

namespace Digikam
{

class Q_DECL_HIDDEN MetadataTask::Private
{
public:

    explicit Private()
        : tagsOnly  (false),
          direction (MetadataSynchronizer::WriteFromDatabaseToFile),
          data      (nullptr)
    {
    }

    bool                                tagsOnly;

    MetadataSynchronizer::SyncDirection direction;

    MaintenanceData*                    data;
};

// -------------------------------------------------------

MetadataTask::MetadataTask()
    : ActionJob(),
      d        (new Private)
{
}

MetadataTask::~MetadataTask()
{
    cancel();
    delete d;
}

void MetadataTask::setTagsOnly(bool value)
{
    d->tagsOnly = value;
}

void MetadataTask::setDirection(MetadataSynchronizer::SyncDirection dir)
{
    d->direction = dir;
}

void MetadataTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void MetadataTask::run()
{
    // While we have data (using this as check for non-null)

    while (d->data)
    {
        if (m_cancel)
        {
            return;
        }

        ItemInfo item = d->data->getItemInfo();

        // If the item is null, we are done.

        if (item.isNull())
        {
            break;
        }

        if (d->direction == MetadataSynchronizer::WriteFromDatabaseToFile)
        {
            MetadataHub hub;

            // read in from database

            hub.load(item);

            // write out to file DMetadata

            if (d->tagsOnly)
            {
                ScanController::FileMetadataWrite writeScope(item);
                writeScope.changed(hub.writeToMetadata(item, MetadataHub::WRITE_TAGS, true));
            }
            else
            {
                ScanController::FileMetadataWrite writeScope(item);
                writeScope.changed(hub.writeToMetadata(item, MetadataHub::WRITE_ALL, true));
            }
        }
        else // MetadataSynchronizer::ReadFromFileToDatabase
        {
            CollectionScanner scanner;
            scanner.scanFile(item, CollectionScanner::CleanScan);
        }

        Q_EMIT signalFinished(QImage());
    }

    Q_EMIT signalDone();
}

} // namespace Digikam
