/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-14
 * Description : Thread actions task for thumbs generator.
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "thumbstask.h"

// Qt includes
#include <QQueue>

// Local includes

#include "digikam_debug.h"
#include "thumbnailloadthread.h"
#include "thumbnailsize.h"
#include "maintenancedata.h"

namespace Digikam
{

class Q_DECL_HIDDEN ThumbsTask::Private
{
public:

    explicit Private()
        : catcher(nullptr),
          data   (nullptr)
    {
    }

    ThumbnailImageCatcher* catcher;

    MaintenanceData*       data;
};

// -------------------------------------------------------

ThumbsTask::ThumbsTask()
    : ActionJob(),
      d        (new Private)
{
    ThumbnailLoadThread* const thread = new ThumbnailLoadThread;
    thread->setPixmapRequested(false);
    thread->setThumbnailSize(ThumbnailLoadThread::maximumThumbnailSize());
    d->catcher                        = new ThumbnailImageCatcher(thread, this);
}

ThumbsTask::~ThumbsTask()
{
    cancel();

    d->catcher->setActive(false);
    d->catcher->thread()->stopAllTasks();

    delete d->catcher->thread();
    delete d->catcher;
    delete d;
}

void ThumbsTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void ThumbsTask::run()
{
    d->catcher->setActive(true);

    // While we have data (using this as check for non-null)
    while (d->data)
    {
        if (m_cancel)
        {
            d->catcher->setActive(false);
            d->catcher->thread()->stopAllTasks();
            return;
        }

        QString path = d->data->getImagePath();

        if (path.isEmpty())
        {
            break;
        }

        // TODO Should be improved by some update procedure
        d->catcher->thread()->deleteThumbnail(path);
        d->catcher->thread()->find(ThumbnailIdentifier(path));
        d->catcher->enqueue();
        QList<QImage> images = d->catcher->waitForThumbnails();
        Q_EMIT signalFinished(images.first());
    }

    Q_EMIT signalDone();
    qDebug() << "ThumbsTask::signalDone()" << this;
    d->catcher->setActive(false);
}

} // namespace Digikam
