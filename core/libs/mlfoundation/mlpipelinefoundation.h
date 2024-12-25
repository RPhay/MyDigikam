/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Foundation for all ML pipelines
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
#include <QIcon>
#include <QAtomicInteger>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadPool>
#include <QFuture>
#include <QFutureWatcher>

// local includes

#include "digikam_export.h"
#include "digikam_opencv.h"
#include "mlpipelinepackagenotify.h"
#include "mlpipelinepackagefoundation.h"

namespace Digikam
{

// class DImg;
class MLPipelinePackage;

template <typename T>
class DIGIKAM_EXPORT SharedQueue;

class DIGIKAM_EXPORT MLPipelineFoundation : public QObject
{
    Q_OBJECT

public:

    enum MLPipelineStage
    {
        /// Finder stage finds the data for the pipeline
        Finder,

        /// Loader stage loads and prepares the data for extraction
        Loader,

        /// Extractor stage pulls the features from the data
        Extractor,

        /// Classifier stage adds a label (face, autotag, etc) to an extracted object
        Classifier,

        /// Classifier stage adds a label (face, autotag, etc) to an extracted object
        Trainer,

        /// Writer stage saves the data to the DB
        Writer,

        // empty stage
        None
    };

    enum MLPipelineNotification
    {
        notifySkipped,
        notifyProcessed
    };

    typedef struct _MLPipelinePerformanceProfile
    {
        int elapsedTime;
        int itemCount;
        int maxQueueCount;
        int maxElapsedTime;
        QAtomicInteger<int> currentThreadCount;
        QAtomicInteger<int> maxThreadCount;
    }
    MLPipelinePerformanceProfile;

    typedef SharedQueue<MLPipelinePackageFoundation*> MLPipelineQueue;

    MLPipelineFoundation();
    virtual ~MLPipelineFoundation();

    virtual bool start();
    virtual void cancel();

    bool hasFinished()      const;

Q_SIGNALS:

    /// Emitted when processing is scheduled.
    void scheduled();

    /// Emitted when processing has started.
    void started(const QString& message);

    /// Emitted when one package begins processing.
    void processing(const MLPipelinePackageNotify::Ptr& package);

    /// Emitted when one package has finished processing.
    void processed(const MLPipelinePackageNotify::Ptr& package);
    void progressValueChanged(float progress);

    /// Emitted when the last package has finished processing.
    void finished();

    /// Emitted when one or several packages were skipped, usually because they have already been scanned.
    void skipped(const MLPipelinePackageNotify::Ptr& package);

    void signalAddMoreWorkers();

    void signalUpdateItemCount(const qlonglong itemCount);

private Q_SLOTS:

    void slotFinished();
    void slotAddMoreWorkers();

protected:

    QMap<MLPipelineStage, MLPipelineQueue*>             queues;
    QMutex                                              mutex;
    QMutex                                              threadStageMutex;
    QAtomicInteger<int>                                 itemsProcessed;
    QAtomicInteger<int>                                 totalItemCount;
    bool                                                cancelled           = false;
    QThreadPool*                                        threadPool          = nullptr;
    QList<QFutureWatcher<bool>* >                       watchList;
    QMap<MLPipelineStage, MLPipelinePerformanceProfile> performanceProfileList;
    quint64                                             maxBufferSize       = 2147483648;           ///< 2 GB default
    quint64                                             usedBufferSize      = 0;

protected:
    virtual bool finder()                                       = 0;
    virtual bool loader()                                       = 0;
    virtual bool extractor()                                    = 0;
    virtual bool classifier()                                   = 0;
    virtual bool trainer()                                      = 0;
    virtual bool writer()                                       = 0;

    virtual void addMoreWorkers()                               = 0;

    // queue helper functions
    MLPipelinePackageFoundation* queueEndSignal() const
    {
        return nullptr;
    }
    void clearQueue(MLPipelineQueue* thisQueue);
    void clearAllQueues();

    virtual bool enqueue(MLPipelineQueue* thisQueue, MLPipelinePackageFoundation* package);
    virtual MLPipelinePackageFoundation* dequeue(MLPipelineQueue* thisQueue);

    bool addWorker(const MLPipelineStage& stage);
    void waitForStart()
    {
        QMutexLocker lock(&mutex);
    }
    void stageStart(QThread::Priority threadPriority, MLPipelineStage thisStage, MLPipelineStage nextStage, MLPipelineQueue*& thisQueue, MLPipelineQueue*& nextQueue);
    void stageEnd(MLPipelineStage thisStage, MLPipelineStage nextStage);

    void notify(MLPipelineNotification notification, const QString _name, const QString _path, int _processed, const QImage& _thumbnail);
    void notify(MLPipelineNotification notification, const QString _name, const QString _path, int _processed, const DImg& _thumbnail);
    void notify(MLPipelineNotification notification, const QString _name, const QString _path, int _processed, const QIcon& _thumbnail);

    void showPipelinePerformance() const;

private:

    MLPipelineFoundation(MLPipelineFoundation&)                 = delete;

};

}