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

#include "databasewriter.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

DatabaseWriter::DatabaseWriter(FacePipeline::WriteMode wmode, FacePipeline::Private* const dd)
    : mode               (wmode),
      thumbnailLoadThread(dd->createThumbnailLoadThread()),
      d                  (dd)
{
}

void DatabaseWriter::process(const FacePipelineExtendedPackage::Ptr& package)
{
    if      (package->databaseFaces.isEmpty())
    {
        // Detection / Recognition

        FaceUtils utils;

        // OverwriteUnconfirmed means that a new scan discarded unconfirmed results of previous scans
        // (assuming at least equal or new, better methodology is in use compared to the previous scan)

        if      ((mode == FacePipeline::OverwriteUnconfirmed) &&
                 (package->processFlags & FacePipelinePackage::ProcessedByDetector))
        {
            QList<FaceTagsIface> oldEntries = utils.unconfirmedFaceTagsIfaces(package->info.id());
            qCDebug(DIGIKAM_GENERAL_LOG) << "Removing old entries" << oldEntries;
            utils.removeFaces(oldEntries);
        }
        else if ((mode == FacePipeline::OverwriteAllFaces) &&
                 (package->processFlags & FacePipelinePackage::ProcessedByDetector))
        {
            utils.removeAllFaces(package->info.id());
        }

        // mark the whole image as scanned-for-faces

        utils.markAsScanned(package->info);

        if (!package->info.isNull() && !package->detectedFaces.isEmpty())
        {
            package->databaseFaces = utils.writeUnconfirmedResults(package->info.id(),
                                                                   package->detectedFaces,
                                                                   package->recognitionResults,
                                                                   package->image.originalSize());
            package->databaseFaces.setRole(FacePipelineFaceTagsIface::DetectedFromImage);

            if (!package->image.isNull())
            {
                utils.storeThumbnails(thumbnailLoadThread, package->filePath,
                                      package->databaseFaces.toFaceTagsIfaceList(), package->image);
            }
        }
    }
    else if (package->processFlags & FacePipelinePackage::ProcessedByRecognizer)
    {
        FaceUtils utils;

        for (int i = 0 ; i < package->databaseFaces.size() ; ++i)
        {
            if (package->databaseFaces[i].roles & FacePipelineFaceTagsIface::ForRecognition)
            {
                // Allow to overwrite existing recognition with new, possibly valid, "not recognized" status

                int tagId = FaceTags::unknownPersonTagId();

                // NOTE: See bug #338485 : check if index is not outside of container size.

                if ((i < package->recognitionResults.size()) &&
                    !package->recognitionResults[i].isNull())
                {
                    // Only perform this call if recognition as results, to prevent crash in QMap. See bug #335624

                    tagId = FaceTags::getOrCreateTagForIdentity(package->recognitionResults[i].attributesMap());
                }

                package->databaseFaces[i]        = FacePipelineFaceTagsIface(utils.changeSuggestedName(package->databaseFaces[i], tagId));
                package->databaseFaces[i].roles &= ~FacePipelineFaceTagsIface::ForRecognition;
           }
        }
    }
    else
    {
        // Editing database entries

        FaceUtils utils;
        FacePipelineFaceTagsIfaceList add;
        FacePipelineFaceTagsIfaceList::iterator it;

        for (it = package->databaseFaces.begin() ; it != package->databaseFaces.end() ; ++it)
        {
            if      (it->roles & FacePipelineFaceTagsIface::ForConfirmation)
            {
                FacePipelineFaceTagsIface confirmed = FacePipelineFaceTagsIface(utils.confirmName(*it, it->assignedTagId, it->assignedRegion));
                confirmed.roles                    |= FacePipelineFaceTagsIface::Confirmed | FacePipelineFaceTagsIface::ForTraining;
                add << confirmed;
            }
            else if (it->roles & FacePipelineFaceTagsIface::ForEditing)
            {
                if      (it->isNull())
                {
                    // add Manually

                    FaceTagsIface newFace = utils.unconfirmedEntry(package->info.id(), it->assignedTagId, it->assignedRegion);
                    utils.addManually(newFace);
                    add << FacePipelineFaceTagsIface(newFace);
                }
                else if (it->assignedRegion.isValid())
                {
                    add << FacePipelineFaceTagsIface(utils.changeRegion(*it, it->assignedRegion));
                }
                else if (FaceTags::isPerson(it->assignedTagId))
                {
                    // Change Tag operation.

                    add << FacePipelineFaceTagsIface(utils.changeTag(*it, it->assignedTagId));
                }
                else
                {
                    utils.removeFace(*it);
                }

                it->roles &= ~FacePipelineFaceTagsIface::ForEditing;
                it->roles |= FacePipelineFaceTagsIface::Edited;
            }

            // Training is done by trainerWorker
        }

        if (!package->image.isNull())
        {
            utils.storeThumbnails(thumbnailLoadThread, package->filePath, add.toFaceTagsIfaceList(), package->image);
        }

        package->databaseFaces << add;
    }

    package->processFlags |= FacePipelinePackage::WrittenToDatabase;

    Q_EMIT processed(package);
}

} // namespace Digikam

#include "moc_databasewriter.cpp"
