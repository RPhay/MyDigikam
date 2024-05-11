/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-14
 * Description : Thread actions task for finger-prints generator.
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "fingerprintstask.h"

// Qt includes

#include <QQueue>
#include <QIcon>

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "haar.h"
#include "haariface.h"
#include "previewloadthread.h"
#include "maintenancedata.h"
#include "similaritydb.h"
#include "similaritydbaccess.h"

namespace Digikam
{

class Q_DECL_HIDDEN FingerprintsTask::Private
{
public:

    Private() = default;

    MaintenanceData* data       = nullptr;
    bool             rebuildAll = true;

    QImage           okImage;
};

// -------------------------------------------------------

FingerprintsTask::FingerprintsTask()
    : ActionJob(),
      d        (new Private)
{
    QPixmap okPix = QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(22, 22);
    d->okImage    = okPix.toImage();
}

FingerprintsTask::~FingerprintsTask()
{
    cancel();

    delete d;
}

void FingerprintsTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void FingerprintsTask::setRebuildAll(bool b)
{
    d->rebuildAll = b;
}

void FingerprintsTask::run()
{
    // While we have data (using this as check for non-null)

    while (d->data)
    {
        if (m_cancel)
        {
            return;
        }

        qlonglong id = d->data->getImageId();

        if (id == -1)
        {
            break;
        }

        ItemInfo info(id);

        if (
            info.isVisible()                                              &&
            (info.category() == DatabaseItem::Category::Image)            &&
            !info.filePath().isEmpty()                                    &&
            !m_cancel                                                     &&
            (
             d->rebuildAll                                                ||
             SimilarityDbAccess().db()->hasDirtyOrMissingFingerprint(info)
            )
           )
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Updating fingerprints for file:" << info.filePath();

            DImg dimg = PreviewLoadThread::loadFastSynchronously(info.filePath(),
                                                                 HaarIface::preferredSize());

            if (!dimg.isNull())
            {
                // compute Haar fingerprint and store it to DB

                HaarIface haarIface;
                haarIface.indexImage(info.id(), dimg);
            }

            QImage qimg = dimg.smoothScale(22, 22, Qt::KeepAspectRatio).copyQImage();

            Q_EMIT signalFinished(qimg);
        }
        else
        {
            Q_EMIT signalFinished(d->okImage);
        }
    }

    Q_EMIT signalDone();
}

} // namespace Digikam

#include "moc_fingerprintstask.cpp"
