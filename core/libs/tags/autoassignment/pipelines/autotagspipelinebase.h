/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Integrated, multithread object detection / recognition
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QImage>
#include <QSemaphore>
#include <QAtomicInteger>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadPool>
#include <QFuture>
#include <QFutureWatcher>
#include <QStringList>
#include <QUrl>

// Local includes

#include "iteminfo.h"
#include "mlpipelinefoundation.h"
#include "autotagsclassifierbase.h"
#include "autotagsscansettings.h"
#include "dnnmodelbase.h"
#include "dnnmodelmanager.h"
#include "dnnmodelnet.h"
#include "dmetadata.h"

namespace Digikam
{

class AutotagsPipelineBase : public MLPipelineFoundation
{
    Q_OBJECT

public:

    explicit AutotagsPipelineBase(const AutotagsScanSettings& _settings);
    virtual ~AutotagsPipelineBase()                                                 override = default;

    virtual bool start()                                                            override;

    virtual void bqmSendOne(QScopedPointer<DMetadata>& _bqmMeta,
                            const ItemInfo& info,
                            const QUrl& outputUrl,
                            const DImg& image);

    virtual void notify(MLPipelineNotification notification,
                        const QString& _name,
                        const QString& _path,
                        const QString& _displayData,
                        int _processed,
                        const QImage& _thumbnail)                                   override;

    virtual void notify(MLPipelineNotification notification,
                        const QString& _name,
                        const QString& _path,
                        const QString& _displayData,
                        int _processed,
                        const DImg& _thumbnail)                                     override;

    virtual void notify(MLPipelineNotification notification,
                        const QString& _name,
                        const QString& _path,
                        const QString& _displayData,
                        int _processed,
                        const QIcon& _thumbnail)                                    override;
protected:

    AutotagsScanSettings        settings;
    DNNModelNet*                model                   = nullptr;
    AutotagsClassifierBase*     autotagsClassifier      = nullptr;
    const QList<AlbumRootInfo>  albumRoots;

    // Batch Queue Manager
    QScopedPointer<DMetadata>   bqmMeta;
    QSemaphore                  bqmSemaphore;
    QUrl                        bqmOutputUrl;

protected:

    // queue helper functions
    bool enqueue(MLPipelineQueue* thisQueue, MLPipelinePackageFoundation* package)  override;

private:

    // Disable
    AutotagsPipelineBase()                            = delete;
    AutotagsPipelineBase(QObject*)                    = delete;
    AutotagsPipelineBase(const AutotagsPipelineBase&) = delete;
};

} // namespace digikam
