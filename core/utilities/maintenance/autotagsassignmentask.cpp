/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Thread actions task for autotags assignment.
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
        : data (nullptr)
    {
    }

    MaintenanceData* data;
    autoTagsAssign*  autotagsEngine;
    QList<QString>   batchImgPaths;
    int              batchSize;
    int              modelType;
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

void AutotagsAssignmentTask::setBatchSize(int batchSize) const
{
    d->batchSize = batchSize;
}

void AutotagsAssignmentTask::setModelType(int modelType) const
{
    d->modelType = modelType;
}

void AutotagsAssignmentTask::assignTags(const QString& pathImage, const QList<QString>& tagsList)
{
    ItemInfo info              = ItemInfo::fromLocalFile(pathImage);

    TagsCache* const tagsCache = Digikam::TagsCache::instance();
    QString rootTags           = QLatin1String("auto/");

    for (const auto& tag : tagsList)
    {
        QString tagPath = rootTags + tag;

        if (!tagsCache->hasTag(tagsCache->tagForPath(tagPath)))
        {
            // TODO: id is unused?
            auto id = tagsCache->createTag(tagPath);
        }

        info.setTag(tagsCache->tagForPath(tagPath));
    }
}

void AutotagsAssignmentTask::setBatchImages(const QList<QString>& batchImgPaths) const
{
    d->batchImgPaths = batchImgPaths;
}

void AutotagsAssignmentTask::run()
{
    if (!m_cancel)
    {
        // Run Autotags backend ere
        // Assign Tags in databae using API from itemInfo

        QElapsedTimer timer;
        timer.start();

        d->autotagsEngine               = new autoTagsAssign(DetectorModel(d->modelType));
        QList<QList<QString>> tagsLists = d->autotagsEngine->generateTagsList(d->batchImgPaths, d->batchSize);

        if (d->batchImgPaths.size() == tagsLists.size())
        {
            for (int i = 0 ; i < d->batchImgPaths.size() ; ++i)
            {
                assignTags(d->batchImgPaths[i], tagsLists[i]);
            }
        }

        int elapsed = timer.elapsed();
        qDebug() << "assgin Tags process takes: " << elapsed << " ms";

        delete d->autotagsEngine;
        d->autotagsEngine = nullptr;
    }

    Q_EMIT signalFinished();

    Q_EMIT signalDone();
}

} // namespace Digikam
