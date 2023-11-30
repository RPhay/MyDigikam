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

    Private() = default;

    MaintenanceData* data      = nullptr;
    int              modelType = DetectorModel::YOLOV5NANO;
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

void AutotagsAssignmentTask::setModelType(int modelType)
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
        int tagId = tagsCache->getOrCreateTag(rootTags + tag);
        info.setTag(tagId);
    }
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

        // Run Autotags backend here
        // Assign Tags in database using API from itemInfo

        QElapsedTimer timer;
        timer.start();

        DImg dimg = PreviewLoadThread::loadHighQualitySynchronously(path, PreviewSettings::RawPreviewAutomatic);

        if (!dimg.isNull())
        {
            AutoTagsAssign* const autotagsEngine = new AutoTagsAssign(DetectorModel(d->modelType));
            QList<QString> tagsList              = autotagsEngine->generateTagsList(dimg);

            if (!tagsList.isEmpty())
            {
                assignTags(path, tagsList);
            }

            delete autotagsEngine;
        }

        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "assgin Tags process takes:"
                                            << timer.elapsed() << "ms";

        QImage qimg = dimg.smoothScale(22, 22, Qt::KeepAspectRatio).copyQImage();

        Q_EMIT signalFinished(qimg);
    }

    Q_EMIT signalDone();
}

} // namespace Digikam
