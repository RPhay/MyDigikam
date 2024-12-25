/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
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

// local includes

#include "mlpipelinefoundation.h"
#include "facescansettings.h"

namespace Digikam
{

class FacePipelineBase : public MLPipelineFoundation
{

public:

    enum FilterMode
    {
        /// Will read any given image.
        ScanAll,

        /// Scan new images, will skip any image that is already marked as scanned.
        ScanNew,

        /// Adds new face(s) to training
        TrainNew,

        /// Retrains the face DB
        TrainAll,

        /// Removes the face(s) from training
        TrainRemove,

        /// Removes all face training, sets all images to not scanned
        TrainReset
    };

    enum WriteMode
    {
        /// Write results. Merge with existing entries.
        NormalWrite,

        /// Add new results. Previous all results will be cleared.
        OverwriteAllFaces,

        /// Add new results. Previous unconfirmed results will be cleared.
        OverwriteUnconfirmed
    };

    explicit FacePipelineBase(const FaceScanSettings& _settings);
    virtual ~FacePipelineBase();

    virtual bool start()    override;
    virtual void cancel()   override;

    // Q_SIGNALS:

    //     /// Emitted when processing is scheduled.
    //     void scheduled();

    //     /// Emitted when processing has started.
    //     void started(const QString& message);

    //     /// Emitted when one package begins processing.
    //     void processing(const FacePipelinePackageNotify::Ptr& package);

    //     /// Emitted when one package has finished processing.
    //     void processed(const FacePipelinePackageNotify::Ptr& package);
    //     void progressValueChanged(float progress);

    //     /// Emitted when the last package has finished processing.
    //     void finished();

    //     /// Emitted when one or several packages were skipped, usually because they have already been scanned.
    //     void skipped(const FacePipelinePackageNotify::Ptr& package);

    //     void signalAddMoreWorkers();

    // private Q_SLOTS:

    //     void slotFinished();
    //     void slotAddMoreWorkers();

protected:

    FaceScanSettings                    settings;

protected:
    bool commonFaceThumbnailLoader(const QString& pipelineName, MLPipelineFoundation::MLPipelineStage thisStage, MLPipelineFoundation::MLPipelineStage nextStage);
    bool commonFaceThumbnailExtractor(const QString& pipelineName, MLPipelineFoundation::MLPipelineStage thisStage, MLPipelineFoundation::MLPipelineStage nextStage);

    // queue helper functions
    bool enqueue(MLPipelineQueue* thisQueue, MLPipelinePackageFoundation* package) override;

private:

    FacePipelineBase()                                          = delete;
    FacePipelineBase(FacePipelineBase&)                         = delete;

};

}