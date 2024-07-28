/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-08
 * Description : FaceEngine database interface allowing easy manipulation of face tags
 *
 * SPDX-FileCopyrightText: 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "faceutils.h"

// Qt includes

#include <QImage>

// Local includes

#include "digikam_debug.h"
#include "coredbaccess.h"
#include "coredbconstants.h"
#include "coredboperationgroup.h"
#include "coredb.h"
#include "dimg.h"
#include "facetags.h"
#include "itemtagpair.h"
#include "tagproperties.h"
#include "tagscache.h"
#include "tagregion.h"
#include "thumbnailloadthread.h"
#include "albummanager.h"
#include "metadatahub.h"

namespace Digikam
{

// --- Constructor / Destructor -------------------------------------------------------------------------------------

FaceUtils::FaceUtils(QObject* const parent)
    : QObject(parent)
{
}

FaceUtils::~FaceUtils()
{
}

// --- Mark for scanning and training -------------------------------------------------------------------------------

bool FaceUtils::hasBeenScanned(qlonglong imageid) const
{
    return hasBeenScanned(ItemInfo(imageid));
}

bool FaceUtils::hasBeenScanned(const ItemInfo& info) const
{
    return info.tagIds().contains(FaceTags::scannedForFacesTagId());
}

void FaceUtils::markAsScanned(qlonglong imageid, bool hasBeenScanned) const
{
    markAsScanned(ItemInfo(imageid), hasBeenScanned);
}

void FaceUtils::markAsScanned(const ItemInfo& info, bool hasBeenScanned) const
{
    if (hasBeenScanned)
    {
        ItemInfo(info).setTag(FaceTags::scannedForFacesTagId());
    }
    else
    {
        ItemInfo(info).removeTag(FaceTags::scannedForFacesTagId());
    }
}

// --- Convert between FacesEngine results and FaceTagsIface ---

QList<FaceTagsIface> FaceUtils::toFaceTagsIfaces(qlonglong imageid,
                                                 const QList<QRectF>& detectedFaces,
                                                 const QList<Identity>& recognitionResults,
                                                 const QSize& fullSize) const
{
    QList<FaceTagsIface> faces;

    for (int i = 0 ; i < detectedFaces.size() ; ++i)
    {
        Identity identity;

        if (!recognitionResults.isEmpty())
        {
            identity = recognitionResults[i];
        }

        // We'll get the unknownPersonTagId if the identity is null

        int tagId                = FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
        QRect fullSizeRect       = TagRegion::relativeToAbsolute(detectedFaces[i], fullSize);
        FaceTagsIface::Type type = identity.isNull() ? FaceTagsIface::UnknownName : FaceTagsIface::UnconfirmedName;

        if (!tagId || !fullSizeRect.isValid())
        {
            faces << FaceTagsIface();
            continue;
        }
/*
        qCDebug(DIGIKAM_GENERAL_LOG) << "New Entry" << fullSizeRect << tagId;
*/
        faces << FaceTagsIface(type, imageid, tagId, TagRegion(fullSizeRect));
    }

    return faces;
}

// --- Images in faces and thumbnails ---

void FaceUtils::storeThumbnails(ThumbnailLoadThread* const thread,
                                const QString& filePath,
                                const QList<FaceTagsIface>& databaseFaces,
                                const DImg& image)
{
    Q_FOREACH (const FaceTagsIface& face, databaseFaces)
    {
        QList<QRect> rects;
        QRect orgRect = face.region().toRect();
        rects << orgRect;
        rects << faceRectToDisplayRect(orgRect);

        Q_FOREACH (const QRect& rect, rects)
        {
            QRect mapped  = TagRegion::mapFromOriginalSize(image, rect);
            QImage detail = image.copyQImage(mapped);
            thread->storeDetailThumbnail(filePath, rect, detail, true);
        }
    }
}

// --- Face detection: merging results ------------------------------------------------------------------------------------

QList<FaceTagsIface> FaceUtils::writeUnconfirmedResults(qlonglong imageid,
                                                        const QList<QRectF>& detectedFaces,
                                                        const QList<Identity>& recognitionResults,
                                                        const QSize& fullSize)
{
    // Build list of new entries

    QList<FaceTagsIface> newFaces = toFaceTagsIfaces(imageid, detectedFaces, recognitionResults, fullSize);

    if (newFaces.isEmpty())
    {
        return newFaces;
    }

    // list of existing entries

    QList<FaceTagsIface> currentFaces = databaseFaces(imageid);

    // merge new with existing entries

    for (int i = 0 ; i < newFaces.size() ; ++i)
    {
        FaceTagsIface& newFace = newFaces[i];
        QList<FaceTagsIface> overlappingEntries;

        Q_FOREACH (const FaceTagsIface& oldFace, currentFaces)
        {
            double minOverlap = oldFace.isConfirmedName() ? 0.25 : 0.5;

            if (oldFace.region().intersects(newFace.region(), minOverlap))
            {
                overlappingEntries << oldFace;
                qCDebug(DIGIKAM_GENERAL_LOG) << "Entry" << oldFace.region() << oldFace.tagId()
                                             << "overlaps" << newFace.region() << newFace.tagId() << ", skipping";
            }
        }

        // The purpose if the next scope is to merge entries:
        // A confirmed face will never be overwritten.
        // If a name is set to an old face, it will only be replaced by a new face with a name.

        if (!overlappingEntries.isEmpty())
        {
            if (newFace.isUnknownName())
            {
                // we have no name in the new face. Do we have one in the old faces?

                for (int j = 0 ; j < overlappingEntries.size() ; ++j)
                {
                    const FaceTagsIface& oldFace = overlappingEntries.at(j);

                    if (oldFace.isUnknownName())
                    {
                        // remove old face
                    }
                    else
                    {
                        // skip new entry if any overlapping face has a name, and we do not

                        newFace = FaceTagsIface();
                        break;
                    }
                }
            }
            else
            {
                // we have a name in the new face. Do we have names in overlapping faces?

                for (int j = 0 ; j < overlappingEntries.size() ; ++j)
                {
                    const FaceTagsIface& oldFace = overlappingEntries[j];

                    if      (oldFace.isUnknownName())
                    {
                        // remove old face
                    }
                    else if (oldFace.isUnconfirmedName())
                    {
                        if (oldFace.tagId() == newFace.tagId())
                        {
                            // remove smaller face

                            if (oldFace.region().intersects(newFace.region(), 1))
                            {
                                newFace = FaceTagsIface();
                                break;
                            }

                            // else remove old face
                        }
                        else
                        {
                            // assume new recognition is more trained, remove older face
                        }
                    }
                    else if (oldFace.isConfirmedName())
                    {
                        // skip new entry, confirmed has of course priority

                        newFace = FaceTagsIface();
                    }
                }
            }
        }

        // if we did not decide to skip this face, add is to the db now

        if (!newFace.isNull())
        {
            // list will contain all old entries that should still be removed

            removeFaces(overlappingEntries);

            ItemTagPair pair(imageid, newFace.tagId());

            // UnconfirmedName and UnknownName have the same attribute

            addFaceAndTag(pair, newFace, FaceTagsIface::attributesForFlags(FaceTagsIface::UnconfirmedName), false);

            // If the face is unconfirmed and the tag is not the unknown person tag, set the unconfirmed person property.

            if (newFace.isUnconfirmedType() && !FaceTags::isTheUnknownPerson(newFace.tagId()))
            {
                ItemTagPair unconfirmedPair(imageid, FaceTags::unconfirmedPersonTagId());
                unconfirmedPair.addProperty(ImageTagPropertyName::autodetectedPerson(),
                                            newFace.getAutodetectedPersonString());
            }
        }
    }

    return newFaces;
}

Identity FaceUtils::identityForTag(int tagId, FacialRecognitionWrapper& recognizer) const
{
    QMultiMap<QString, QString> attributes = FaceTags::identityAttributes(tagId);
    Identity identity                      = recognizer.findIdentity(attributes);

    if (!identity.isNull())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Found FacesEngine identity" << identity.id() << "for tag" << tagId;
        return identity;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Adding new FacesEngine identity with attributes" << attributes;
    identity                               = recognizer.addIdentity(attributes);

    FaceTags::applyTagIdentityMapping(tagId, identity.attributesMap());

    return identity;
}

int FaceUtils::tagForIdentity(const Identity& identity) const
{
    return FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
}

// --- Editing normal tags, reimplemented with MetadataHub ---

void FaceUtils::addNormalTag(qlonglong imageId, int tagId)
{
    FaceTagsEditor::addNormalTag(imageId, tagId);

    ItemInfo info(imageId);
    MetadataHub hub;
    hub.load(info);

    hub.writeToMetadata(info, MetadataHub::WRITE_TAGS);
}

void FaceUtils::removeNormalTag(qlonglong imageId, int tagId)
{
    FaceTagsEditor::removeNormalTag(imageId, tagId);

    ItemInfo info(imageId);
    MetadataHub hub;
    hub.load(info);

    hub.writeToMetadata(info, MetadataHub::WRITE_TAGS);

    if (
        !FaceTags::isTheIgnoredPerson(tagId)  &&
        !FaceTags::isTheUnknownPerson(tagId)  &&
        !FaceTags::isTheUnconfirmedPerson(tagId)
       )
    {
        qlonglong faceItemId = CoreDbAccess().db()->getFirstItemWithFaceTag(tagId);

        /**
         * If the face just removed was the final face
         * associated with that Tag, reset Tag Icon.
         */
        if (faceItemId == -1)
        {
            TAlbum* const album = AlbumManager::instance()->findTAlbum(tagId);

            if (album && (album->iconId() != 0))
            {
                QString err;

                if (!AlbumManager::instance()->updateTAlbumIcon(album, QString(),
                                                                0, err))
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << err ;
                }
            }
        }
    }
}

void FaceUtils::removeNormalTags(qlonglong imageId, const QList<int>& tagIds)
{
    FaceTagsEditor::removeNormalTags(imageId, tagIds);

    ItemInfo info(imageId);
    MetadataHub hub;
    hub.load(info);

    hub.writeToMetadata(info, MetadataHub::WRITE_TAGS);
}

// --- Utilities ---

QRect FaceUtils::faceRectToDisplayRect(const QRect& rect)
{
    /*
     * Do not change that value unless you know what you do.
     * There are a lot of pregenerated thumbnails in user's databases,
     * expensive to regenerate, depending on this very value.
     */
    int margin = qMax(rect.width(), rect.height());
    margin    /= 10;

    return rect.adjusted(-margin, -margin, margin, margin);
}

} // Namespace Digikam

#include "moc_faceutils.cpp"
