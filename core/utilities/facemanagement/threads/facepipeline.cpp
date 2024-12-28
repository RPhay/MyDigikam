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
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipeline_p.h"

// Local includes

#include "databasewriter.h"
#include "detectionworker.h"
#include "recognitionworker.h"
#include "trainerworker.h"
#include "faceitemretriever.h"
#include "recognitionbenchmarker.h"
#include "detectionbenchmarker.h"
#include "facepreviewloader.h"
#include "parallelpipes.h"
#include "scanstatefilter.h"

namespace Digikam
{

FacePipeline::FacePipeline()
    : d(new Private(this))
{
    qRegisterMetaType<FacePipelineExtendedPackage::Ptr>("FacePipelineExtendedPackage::Ptr");
    qRegisterMetaType<FaceScanSettings::FaceDetectionModel>("FaceScanSettings::FaceDetectionModel");
    qRegisterMetaType<FaceScanSettings::FaceDetectionSize>("FaceScanSettings::FaceDetectionSize");
    qRegisterMetaType<FaceScanSettings::FaceRecognitionModel>("FaceScanSettings::FaceRecognitionModel");
}

FacePipeline::~FacePipeline()
{
    shutDown();

    delete d->databaseFilter;
    delete d->previewThread;
    delete d->detectionWorker;
    delete d->parallelDetectors;
    delete d->recognitionWorker;
    delete d->databaseWriter;
    delete d->trainerWorker;
    qDeleteAll(d->thumbnailLoadThreads);
    delete d->detectionBenchmarker;
    delete d->recognitionBenchmarker;
    delete d;
}

void FacePipeline::shutDown()
{
    cancel();
    d->wait();
}

bool FacePipeline::hasFinished() const
{
    return d->hasFinished();
}

QString FacePipeline::benchmarkResult() const
{
    if (d->detectionBenchmarker)
    {
        return d->detectionBenchmarker->result();
    }

    if (d->recognitionBenchmarker)
    {
        return d->recognitionBenchmarker->result();
    }

    return QString();
}

void FacePipeline::plugDatabaseFilter(FilterMode mode)
{
    d->databaseFilter = new ScanStateFilter(mode, d);
}

void FacePipeline::plugRerecognizingDatabaseFilter()
{
    plugDatabaseFilter(ReadUnconfirmedFaces);
    d->databaseFilter->tasks = FacePipelineFaceTagsIface::ForRecognition;
}

void FacePipeline::plugRetrainingDatabaseFilter()
{
    plugDatabaseFilter(ReadConfirmedFaces);
    d->databaseFilter->tasks = FacePipelineFaceTagsIface::ForTraining;
}

void FacePipeline::plugFacePreviewLoader()
{
    d->previewThread = new FacePreviewLoader(d);
}

void FacePipeline::plugFaceDetector()
{
    d->detectionWorker = new DetectionWorker(d);

    connect(d, SIGNAL(accuracyAndModel(int,FaceScanSettings::FaceDetectionModel,FaceScanSettings::FaceDetectionSize,int,FaceScanSettings::FaceRecognitionModel)),
            d->detectionWorker, SLOT(setAccuracyAndModel(int,FaceScanSettings::FaceDetectionModel,FaceScanSettings::FaceDetectionSize,int,FaceScanSettings::FaceRecognitionModel)),
            Qt::QueuedConnection);
}

void FacePipeline::plugParallelFaceDetectors()
{
    if (QThread::idealThreadCount() <= 1)
    {
        plugFaceDetector();
        return;
    }

    // Limit number of parallel detectors to 3, because of memory cost (cascades).

    const int n          = qMin(3, QThread::idealThreadCount());
    d->parallelDetectors = new ParallelPipes;

    for (int i = 0 ; i < n ; ++i)
    {
        DetectionWorker* const worker = new DetectionWorker(d);

        connect(d, SIGNAL(accuracyAndModel(int,FaceScanSettings::FaceDetectionModel,FaceScanSettings::FaceDetectionSize,int,FaceScanSettings::FaceRecognitionModel)),
                worker, SLOT(setAccuracyAndModel(int,FaceScanSettings::FaceDetectionModel,FaceScanSettings::FaceDetectionSize,int,FaceScanSettings::FaceRecognitionModel)),
                Qt::QueuedConnection);

        d->parallelDetectors->add(worker);
    }
}

void FacePipeline::plugFaceRecognizer()
{
    d->recognitionWorker = new RecognitionWorker(d);

    connect(d, SIGNAL(accuracyAndModel(int,FaceScanSettings::FaceDetectionModel,FaceScanSettings::FaceDetectionSize,int,FaceScanSettings::FaceRecognitionModel)),
            d->recognitionWorker, SLOT(setAccuracyAndModel(int,FaceScanSettings::FaceDetectionModel,FaceScanSettings::FaceDetectionSize,int,FaceScanSettings::FaceRecognitionModel)),
            Qt::QueuedConnection);
}

void FacePipeline::plugDatabaseWriter(WriteMode mode)
{
    d->databaseWriter = new DatabaseWriter(mode, d);
}

void FacePipeline::plugTrainer()
{
    d->trainerWorker = new TrainerWorker(d);
}

void FacePipeline::plugDetectionBenchmarker()
{
    d->detectionBenchmarker = new DetectionBenchmarker(d);
}

void FacePipeline::plugRecognitionBenchmarker()
{
    d->recognitionBenchmarker = new RecognitionBenchmarker(d);
}

void FacePipeline::plugDatabaseEditor()
{
    plugDatabaseWriter(NormalWrite);
}

void FacePipeline::construct()
{
    if (d->previewThread)
    {
        d->pipeline << d->previewThread;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add preview thread";
    }

    if      (d->parallelDetectors)
    {
        d->pipeline << d->parallelDetectors;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add parallel thread detectors";
    }
    else if (d->detectionWorker)
    {
        d->pipeline << d->detectionWorker;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add single thread detector";
    }

    if (d->recognitionWorker)
    {
        d->pipeline << d->recognitionWorker;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add recognition worker";
    }

    if (d->detectionBenchmarker)
    {
        d->pipeline << d->detectionBenchmarker;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add detection benchmaker";
    }

    if (d->recognitionBenchmarker)
    {
        d->pipeline << d->recognitionBenchmarker;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add recognition benchmaker";
    }

    if (d->databaseWriter)
    {
        d->pipeline << d->databaseWriter;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add database writer";
    }

    if (d->trainerWorker)
    {
        d->pipeline << d->trainerWorker;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Face PipeLine: add faces trainer";
    }

    if (d->pipeline.isEmpty())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Nothing plugged in. It is a noop.";
        return;
    }

    connect(d, SIGNAL(startProcess(FacePipelineExtendedPackage::Ptr)),
            d->pipeline.first(), SLOT(process(FacePipelineExtendedPackage::Ptr)),
            Qt::QueuedConnection);

    for (int i = 0 ; i < (d->pipeline.size() - 1) ; ++i)
    {
        connect(d->pipeline.at(i), SIGNAL(processed(FacePipelineExtendedPackage::Ptr)),
                d->pipeline.at(i + 1), SLOT(process(FacePipelineExtendedPackage::Ptr)),
                Qt::QueuedConnection);
    }

    connect(d->pipeline.last(), SIGNAL(processed(FacePipelineExtendedPackage::Ptr)),
            d, SLOT(finishProcess(FacePipelineExtendedPackage::Ptr)),
            Qt::QueuedConnection);

    d->applyPriority();
}

void FacePipeline::setPriority(QThread::Priority priority)
{
    if (d->priority == priority)
    {
        return;
    }

    d->priority = priority;
    d->applyPriority();
}

QThread::Priority FacePipeline::priority() const
{
    return d->priority;
}

void FacePipeline::cancel()
{
    d->stop();
}

bool FacePipeline::process(const ItemInfo& info)
{
    QString filePath = info.filePath();

    if (filePath.isNull())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "ItemInfo has no valid file path. Skipping.";
        return false;
    }

    FacePipelineExtendedPackage::Ptr package = d->filterOrBuildPackage(info);

    if (!package)
    {
        return false;
    }

    d->send(package);

    return true;
}

bool FacePipeline::process(const ItemInfo& info,
                           const DImg& image)
{
    FacePipelineExtendedPackage::Ptr package = d->filterOrBuildPackage(info);

    if (!package)
    {
        return false;
    }

    package->image = image;
    d->send(package);

    return true;
}

void FacePipeline::train(const ItemInfo& info,
                         const QList<FaceTagsIface>& databaseFaces)
{
    train(info, databaseFaces, DImg());
}

void FacePipeline::train(const ItemInfo& info,
                         const QList<FaceTagsIface>& databaseFaces,
                         const DImg& image)
{
    FacePipelineExtendedPackage::Ptr package = d->buildPackage(info,
                                                               FacePipelineFaceTagsIfaceList(databaseFaces),
                                                               image);
    package->databaseFaces.setRole(FacePipelineFaceTagsIface::ForTraining);
    d->send(package);
}

FaceTagsIface FacePipeline::confirm(const ItemInfo& info,
                                    const FaceTagsIface& databaseFace,
                                    int assignedTagId,
                                    const TagRegion& assignedRegion)
{
    return confirm(info, databaseFace, DImg(), assignedTagId, assignedRegion);
}

FaceTagsIface FacePipeline::confirm(const ItemInfo& info,
                                    const FaceTagsIface& databaseFace,
                                    const DImg& image,
                                    int assignedTagId,
                                    const TagRegion& assignedRegion)
{
    FacePipelineFaceTagsIface face            = FacePipelineFaceTagsIface(databaseFace);
    face.assignedTagId                        = assignedTagId;
    face.assignedRegion                       = assignedRegion;
    face.roles                               |= FacePipelineFaceTagsIface::ForConfirmation;
    FacePipelineExtendedPackage::Ptr package  = d->buildPackage(info, face, image);

    d->send(package);

    return FaceTagsEditor::confirmedEntry(face, assignedTagId, assignedRegion);
}

FaceTagsIface FacePipeline::addManually(const ItemInfo& info,
                                        const DImg& image,
                                        const TagRegion& assignedRegion)
{
    // NOTE: giving a null face => no existing face yet, add it.

    FacePipelineFaceTagsIface face;
    face.assignedTagId                        = -1;
    face.assignedRegion                       = assignedRegion;
    face.roles                               |= FacePipelineFaceTagsIface::ForEditing;
    FacePipelineExtendedPackage::Ptr package  = d->buildPackage(info, face, image);

    package->databaseFaces.setRole(FacePipelineFaceTagsIface::ForEditing);
    d->send(package);

    return FaceTagsEditor::unconfirmedEntry(info.id(), face.assignedTagId, face.assignedRegion);
}

FaceTagsIface FacePipeline::editRegion(const ItemInfo& info,
                                       const DImg& image,
                                       const FaceTagsIface& databaseFace,
                                       const TagRegion& newRegion)
{
    FacePipelineFaceTagsIface face            = FacePipelineFaceTagsIface(databaseFace);
    face.assignedTagId                        = -1;
    face.assignedRegion                       = newRegion;
    face.roles                               |= FacePipelineFaceTagsIface::ForEditing;
    FacePipelineExtendedPackage::Ptr package  = d->buildPackage(info, face, image);

    package->databaseFaces.setRole(FacePipelineFaceTagsIface::ForEditing);
    d->send(package);

    face.setRegion(newRegion);

    return face;
}

FaceTagsIface FacePipeline::editTag(const ItemInfo& info,
                                    const FaceTagsIface& databaseFace,
                                    int newTagId)
{
    FacePipelineFaceTagsIface face           = FacePipelineFaceTagsIface(databaseFace);
    face.assignedTagId                       = newTagId;
    face.assignedRegion                      = TagRegion();
    face.roles                              |= FacePipelineFaceTagsIface::ForEditing;

    FacePipelineExtendedPackage::Ptr package = d->buildPackage(info, face, DImg());

    package->databaseFaces.setRole(FacePipelineFaceTagsIface::ForEditing);
    d->send(package);

    return face;
}

void FacePipeline::remove(const ItemInfo& info,
                          const FaceTagsIface& databaseFace)
{
    FacePipelineExtendedPackage::Ptr package = d->buildPackage(info,
                                                               FacePipelineFaceTagsIface(databaseFace),
                                                               DImg());
    package->databaseFaces.setRole(FacePipelineFaceTagsIface::ForEditing);
    d->send(package);
}

void FacePipeline::process(const QList<ItemInfo>& infos)
{
    d->processBatch(infos);
}

void FacePipeline::setAccuracyAndModel(int detectAccuracy,
                                       FaceScanSettings::FaceDetectionModel detectModel,
                                       FaceScanSettings::FaceDetectionSize detectSize,
                                       int recognizeAccuracy,
                                       FaceScanSettings::FaceRecognitionModel recognizeModel)
{
    Q_EMIT d->accuracyAndModel(detectAccuracy, detectModel, detectSize, recognizeAccuracy, recognizeModel);
}

} // namespace Digikam

#include "moc_facepipeline.cpp"
