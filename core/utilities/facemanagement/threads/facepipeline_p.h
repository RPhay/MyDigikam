/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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

    explicit Private(FacePipeline* const qq);

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

    bool hasFinished() const;
    void checkFinished();
    void start();
    void stop();
    void wait();
    void applyPriority();

    ThumbnailLoadThread* createThumbnailLoadThread();

public:

    ScanStateFilter*                        databaseFilter          = nullptr;
    FacePreviewLoader*                      previewThread           = nullptr;
    DetectionWorker*                        detectionWorker         = nullptr;
    ParallelPipes*                          parallelDetectors       = nullptr;
    RecognitionWorker*                      recognitionWorker       = nullptr;
    DatabaseWriter*                         databaseWriter          = nullptr;
    TrainerWorker*                          trainerWorker           = nullptr;
    DetectionBenchmarker*                   detectionBenchmarker    = nullptr;
    RecognitionBenchmarker*                 recognitionBenchmarker  = nullptr;

    QList<QObject*>                         pipeline;
    QThread::Priority                       priority                = QThread::LowPriority;

    QList<ThumbnailLoadThread*>             thumbnailLoadThreads;
    bool                                    started                 = false;
    bool                                    waiting                 = false;
    int                                     infosForFiltering       = 0;
    int                                     packagesOnTheRoad       = 0;
    int                                     maxPackagesOnTheRoad    = 30;
    int                                     totalPackagesAdded      = 0;

    QList<FacePipelineExtendedPackage::Ptr> delayedPackages;

public Q_SLOTS:

    void finishProcess(FacePipelineExtendedPackage::Ptr package);

Q_SIGNALS:

    friend class FacePipeline;
    void startProcess(const FacePipelineExtendedPackage::Ptr& package);

    void accuracyAndModel(double accuracy, bool yolo);
    void thresholdChanged(double threshold);

private:

    FacePipeline* const q = nullptr;
};

} // namespace Digikam
