/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "faceitemretriever.h"

namespace Digikam
{

FaceItemRetriever::FaceItemRetriever(FacePipeline::Private* const d)
    : catcher(new ThumbnailImageCatcher(d->createThumbnailLoadThread(), d))
{
}

FaceItemRetriever::~FaceItemRetriever()
{
}

void FaceItemRetriever::cancel()
{
    catcher->cancel();
}

QList<QImage*> FaceItemRetriever::getDetails(const DImg& src, const QList<QRectF>& rects) const
{
    QList<QImage*> images;

    for (const QRectF& rect : std::as_const(rects))
    {
        QImage* const croppedFace = new QImage();
        (*croppedFace)            = src.copyQImage(rect);

        images << croppedFace;
    }

    return images;
}

QList<QImage*> FaceItemRetriever::getDetails(const DImg& src, const QList<FaceTagsIface>& faces) const
{
    QList<QImage*> images;

    for (const FaceTagsIface& face : std::as_const(faces))
    {
        QRect rect                = TagRegion::mapFromOriginalSize(src, face.region().toRect());

        QImage* const croppedFace = new QImage();
        (*croppedFace)            = src.copyQImage(rect);

        images << croppedFace;
    }

    return images;
}

QList<QImage*> FaceItemRetriever::getThumbnails(const QString& filePath, const QList<FaceTagsIface>& faces) const
{
    Q_UNUSED(filePath)
    catcher->setActive(true);

    for (const FaceTagsIface& face : std::as_const(faces))
    {
        QRect rect = face.region().toRect();
        catcher->thread()->find(ItemInfo::thumbnailIdentifier(face.imageId()), rect);
        catcher->enqueue();
    }

    QList<QImage> images = catcher->waitForThumbnails();

    QList<QImage*> croppedFaces;

    for (int i = 0 ; i < images.size() ; ++i)
    {
        QImage* const croppedFace = new QImage();
        (*croppedFace)            = images[i].copy();

        croppedFaces << croppedFace;
    }

    catcher->setActive(false);

    return croppedFaces;
}

} // namespace Digikam
