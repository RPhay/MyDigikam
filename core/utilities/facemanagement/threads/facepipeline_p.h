/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_PIPELINE_PRIVATE_H
#define DIGIKAM_FACE_PIPELINE_PRIVATE_H

#include "facepipeline.h"

// Qt includes

#include <QExplicitlySharedDataPointer>
#include <QMetaMethod>
#include <QMutex>
#include <QSharedData>
#include <QWaitCondition>

// Local includes

#include "facedetector.h"
#include "faceutils.h"
#include "previewloadthread.h"
#include "thumbnailloadthread.h"
#include "workerobject.h"

namespace Digikam
{

class Q_DECL_HIDDEN FacePipeline::Private : public QObject
{
    Q_OBJECT

public:

    explicit Private(FacePipeline* const q);

    void processBatch(const QList<ItemInfo>& infos);
    void sendFromFilter(const QList<FacePipelineExtendedPackage::Ptr>& packages);
    void skipFromFilter(const QList<ItemInfo>& infosForSkipping);
    void send(const FacePipelineExtendedPackage::Ptr& package);
    bool senderFlowControl(const FacePipelineExtendedPackage::Ptr& package);
    void receiverFlowControl();
    FacePipelineExtendedPackage::Ptr buildPackage(const ItemInfo& info);
    FacePipelineExtendedPackage::Ptr buildPackage(const ItemInfo& info,
                                                  const FacePipelineFaceTagsIface&,
                                                  const DImg& image);
    FacePipelineExtendedPackage::Ptr buildPackage(const ItemInfo& info,
                                                  const FacePipelineFaceTagsIfaceList& faces,
                                                  const DImg& image);
    FacePipelineExtendedPackage::Ptr filterOrBuildPackage(const ItemInfo& info);

    bool hasFinished();
    void checkFinished();
    void start();
    void stop();
    void wait();
    void applyPriority();

    ThumbnailLoadThread* createThumbnailLoadThread();

public:

    ScanStateFilter*                        databaseFilter;
    FacePreviewLoader*                      previewThread;
    DetectionWorker*                        detectionWorker;
    ParallelPipes*                          parallelDetectors;
    RecognitionWorker*                      recognitionWorker;
    DatabaseWriter*                         databaseWriter;
    TrainerWorker*                          trainerWorker;
    DetectionBenchmarker*                   detectionBenchmarker;
    RecognitionBenchmarker*                 recognitionBenchmarker;

    QList<QObject*>                         pipeline;
    QThread::Priority                       priority;

    QList<ThumbnailLoadThread*>             thumbnailLoadThreads;
    bool                                    started;
    bool                                    waiting;
    int                                     infosForFiltering;
    int                                     packagesOnTheRoad;
    int                                     maxPackagesOnTheRoad;
    int                                     totalPackagesAdded;

    QList<FacePipelineExtendedPackage::Ptr> delayedPackages;

public Q_SLOTS:

    void finishProcess(FacePipelineExtendedPackage::Ptr package);

Q_SIGNALS:

    friend class FacePipeline;
    void startProcess(FacePipelineExtendedPackage::Ptr package);

    void accuracyAndModel(double accuracy, bool yolo);
    void thresholdChanged(double threshold);

private:

    FacePipeline* const q;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_PIPELINE_PRIVATE_H
