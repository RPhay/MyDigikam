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

#include "trainerworker.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN MapListTrainingDataProvider : public TrainingDataProvider
{
public:

    MapListTrainingDataProvider() = default;

    ~MapListTrainingDataProvider() override
    {
        qDeleteAll(imagesToTrain);
        imagesToTrain.clear();
    }

    ImageListProvider* newImages(const Identity& identity) override
    {
        if (imagesToTrain.contains(identity.id()))
        {
            QListImageListProvider* const provider = imagesToTrain.value(identity.id());

            if (provider)
            {
                provider->reset();
            }

            return provider;
        }

        return &empty;
    }

    ImageListProvider* images(const Identity&) override
    {
        // Not implemented. Would be needed if we use a backend with a "holistic" approach that needs all images to train.

        return &empty;
    }

public:

    EmptyImageListProvider             empty;
    QMap<int, QListImageListProvider*> imagesToTrain;
};

// ----------------------------------------------------------------------------------------

TrainerWorker::TrainerWorker(FacePipeline::Private* const dd)
    : imageRetriever(dd),
      d             (dd)
{
}

TrainerWorker::~TrainerWorker()
{
    wait();    // protect detector
}

/**
 * TODO: investigate this method
 */
void TrainerWorker::process(const FacePipelineExtendedPackage::Ptr& package)
{
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "TrainerWorker: processing one package";
*/
    // Get a list of faces with type FaceForTraining (probably type is ConfirmedFace)

    QList<FaceTagsIface> toTrain;
    QList<int>           identities;
    QList<Identity>      identitySet;
    FaceUtils            utils;

    Q_FOREACH (const FacePipelineFaceTagsIface& face, package->databaseFaces)
    {
        if (face.roles & FacePipelineFaceTagsIface::ForTraining)
        {
            FaceTagsIface dbFace = face;
            qCDebug(DIGIKAM_GENERAL_LOG) << "Test1 FaceTagsIface:" << face.imageId() << face.tagId() << dbFace.imageId() << dbFace.tagId();
            dbFace.setType(FaceTagsIface::FaceForTraining);
            toTrain << dbFace;
            qCDebug(DIGIKAM_GENERAL_LOG) << "Test2 FaceTagsIface:" << toTrain.last().imageId() << toTrain.last().tagId();
            Identity identity    = utils.identityForTag(dbFace.tagId(), recognizer);

            identities << identity.id();

            if (!identitySet.contains(identity))
            {
                identitySet << identity;
            }
        }
    }

    if (!toTrain.isEmpty())
    {
        QList<QImage*> images;

        if (package->image.isNull())
        {
            images = imageRetriever.getThumbnails(package->filePath, toTrain);
        }
        else
        {
            images = imageRetriever.getDetails(package->image, toTrain);
        }

        MapListTrainingDataProvider provider;

        // Group images by identity

        for (int i = 0 ; i < toTrain.size() ; ++i)
        {
            QListImageListProvider* const imageList = new QListImageListProvider;
            imageList->setImages(QList<QImage*>() << images[i]);
            provider.imagesToTrain[identities[i]] = imageList;
        }

        // NOTE: cropped faces will be deleted by training provider

        recognizer.train(identitySet, &provider, QLatin1String("digikam"));
    }

    utils.removeFaces(toTrain);
    package->databaseFaces.replaceRole(FacePipelineFaceTagsIface::ForTraining, FacePipelineFaceTagsIface::Trained);
    package->processFlags |= FacePipelinePackage::ProcessedByTrainer;

    Q_EMIT processed(package);
}

void TrainerWorker::aboutToDeactivate()
{
    imageRetriever.cancel();
}

} // namespace Digikam

#include "moc_trainerworker.cpp"
