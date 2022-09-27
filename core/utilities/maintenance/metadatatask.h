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

#ifndef DIGIKAM_METADATA_TASK_H
#define DIGIKAM_METADATA_TASK_H

// Qt includes

#include <QThread>

// Local includes

#include "actionthreadbase.h"
#include "iteminfo.h"
#include "metadatasynchronizer.h"

namespace Digikam
{

class MaintenanceData;

class MetadataTask : public ActionJob
{
    Q_OBJECT

public:

    explicit MetadataTask();
    ~MetadataTask()         override;

    void setTagsOnly(bool value);
    void setDirection(MetadataSynchronizer::SyncDirection dir);
    void setMaintenanceData(MaintenanceData* const data = nullptr);

Q_SIGNALS:

    void signalFinished(const QImage&);

protected:

    void run()              override;

private:

    // Disable
    MetadataTask(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_METADATA_TASK_H
