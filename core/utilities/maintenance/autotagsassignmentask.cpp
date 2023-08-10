/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Thread actions task for image quality sorter.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsassignmentask.h"

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "previewloadthread.h"
#include "iteminfo.h"
#include "maintenancedata.h"
#include "autotagsassign.h"
#include "tagscache.h"

namespace Digikam
{

class Q_DECL_HIDDEN AutotagsAssignmentTask::Private
{
public:

    explicit Private()
        : data    (nullptr)
    {
    }

    MaintenanceData*      data;
    autoTagsAssign*       autotagsEngine;
};

// -------------------------------------------------------

AutotagsAssignmentTask::AutotagsAssignmentTask()
    : ActionJob(),
      d        (new Private)
{
}

AutotagsAssignmentTask::~AutotagsAssignmentTask()
{
    cancel();

    delete d;
}

void AutotagsAssignmentTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void AutotagsAssignmentTask::run()
{
    // While we have data (using this as check for non-null)
    
    while (d->data)
    {
        if (m_cancel)
        {
            return;
        }

        QString path = d->data->getImagePath();

        if (path.isEmpty())
        {
            break;
        }

        if (!m_cancel)
        {
            // Run Autotags backend here
            // Assign Tags in database using API from itemInfo
            ItemInfo info = ItemInfo::fromLocalFile(path);
            d->autotagsEngine = new autoTagsAssign();

            QList<QString> tagsList = d->autotagsEngine->generateTagsList(path);
            TagsCache* tagsCache = Digikam::TagsCache::instance();

            QString rootTags = QLatin1String("auto/");
            for (auto tag : tagsList)
            {
                QString tagPath = rootTags + tag;
                if (!tagsCache->hasTag(tagsCache->tagForPath(tagPath)))
                {
                    auto id = tagsCache->createTag(tagPath);
                }

                info.setTag(tagsCache->tagForPath(tagPath));
            }

            d->autotagsEngine = nullptr;
            delete d->autotagsEngine;
        }


        Q_EMIT signalFinished();
    }

    Q_EMIT signalDone();
}

} // namespace Digikam
