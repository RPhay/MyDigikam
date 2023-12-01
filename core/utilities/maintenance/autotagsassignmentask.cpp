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

    const int loadCount        = 3;
    const int batchSize        = 16;
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

        QList<DImg> inputImages;

        for (int i = 0 ; i < d->loadCount ; ++i)
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

            inputImages << PreviewLoadThread::loadFastSynchronously(path, 1000);
        }

        if (inputImages.isEmpty())
        {
            break;
        }

        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Current batch size:" << inputImages.size();

        // Run Autotags backend here
        // Assign Tags in database using API from itemInfo

        QElapsedTimer timer;
        timer.start();

        QScopedPointer<AutoTagsAssign> autotagsEngine(new AutoTagsAssign(DetectorModel(d->modelType)));
        QList<QList<QString> >tagsLists = autotagsEngine->generateTagsList(inputImages, d->batchSize);

        for (int j = 0 ; j < inputImages.size() ; ++j)
        {
            QImage qimg = inputImages.at(j).smoothScale(22, 22, Qt::KeepAspectRatio).copyQImage();
            assignTags(inputImages.at(j).originalFilePath(), tagsLists.at(j));

            Q_EMIT signalFinished(qimg);
        }

        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Assgin Tags process takes:"
                                            << timer.elapsed() << "ms";
    }

    Q_EMIT signalDone();
}

} // namespace Digikam
