/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-21-06
 * Description : Thread actions task for metadata remover.
 *
 * SPDX-FileCopyrightText: 2013-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2023      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "metadataremovetask.h"

// Local includes

#include "digikam_debug.h"
#include "maintenancedata.h"
#include "faceutils.h"
#include "iteminfo.h"

namespace Digikam
{

class Q_DECL_HIDDEN MetadataRemoveTask::Private
{
public:

    explicit Private()
        : removeAction (MetadataRemover::None),
          data         (nullptr)
    {
    }

    MetadataRemover::RemoveAction removeAction;

    MaintenanceData*              data;
};

// -------------------------------------------------------

MetadataRemoveTask::MetadataRemoveTask()
    : ActionJob(),
      d        (new Private)
{
}

MetadataRemoveTask::~MetadataRemoveTask()
{
    cancel();
    delete d;
}

void MetadataRemoveTask::setRemoveAction(MetadataRemover::RemoveAction action)
{
    d->removeAction = action;
}

void MetadataRemoveTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void MetadataRemoveTask::run()
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

        if (d->removeAction == MetadataRemover::Faces)
        {
            if (FaceUtils().databaseFaces(item.id()).size() > 0)
            {
                FaceUtils().removeAllFaces(item.id());
            }
        }

        Q_EMIT signalFinished(QImage());
    }

    Q_EMIT signalDone();
}

} // namespace Digikam
