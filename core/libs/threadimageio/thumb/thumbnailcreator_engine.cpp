/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-07-20
 * Description : Loader for thumbnails - thumbnail generation and image handling
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "thumbnailcreator_p.h"

namespace Digikam
{

ThumbnailImage ThumbnailCreator::createThumbnail(const ThumbnailInfo& info, const QRect& detailRect) const
{
    const QString path = info.filePath;
    QFileInfo fileInfo(path);

    if (!info.isAccessible || !fileInfo.exists() || !fileInfo.isFile())
    {
        d->error = i18n("File does not exist or is not a file");
        return ThumbnailImage();
    }

    QImage qimage;
    QScopedPointer<DMetadata> metadata(new DMetadata(path));
    bool fromEmbeddedPreview = false;
    bool fromDetail          = false;
    bool failedAtDImg        = false;
    bool failedAtJPEGScaled  = false;
    bool failedAtPGFScaled   = false;

    // -- Get the image preview --------------------------------

    IccProfile profile;
    bool colorManage = IccSettings::instance()->useManagedPreviews();

    if (!detailRect.isNull())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to get thumbnail with details for" << path;

        // When taking a detail, we have to load the image full size

        qimage     = loadImageDetail(info, *metadata, detailRect, &profile);
        fromDetail = !qimage.isNull();
    }
    else
    {
        if (info.mimeType == QLatin1String("image"))
        {
            if (qimage.isNull())
            {
                // Try to extract Exif/IPTC preview first.

                qimage = loadImagePreview(*metadata);
            }

            if (d->observer && !d->observer->continueQuery())
            {
                return ThumbnailImage();
            }

            // To speed-up thumb extraction, we now try to load the images by the file extension.

            QString ext = fileInfo.suffix().toUpper();

            if (qimage.isNull() && !ext.isEmpty())
            {
                if      ((ext == QLatin1String("JPEG")) ||
                         (ext == QLatin1String("JPG"))  ||
                         (ext == QLatin1String("JPE")))
                {
                    if (colorManage)
                    {
                        qimage = loadWithDImgScaled(path, &profile);
                    }
                    else
                    {
                        // Use jpegutils

                        JPEGUtils::loadJPEGScaled(qimage, path, d->storageSize());
                    }

                    failedAtJPEGScaled = qimage.isNull();
                }
                else if ((ext == QLatin1String("PNG"))  ||
                         (ext == QLatin1String("TIFF")) ||
                         (ext == QLatin1String("TIF")))
                {
                    // Use DImg load scaled mode

                    qimage       = loadWithDImgScaled(path, &profile);
                    failedAtDImg = qimage.isNull();
                }
                else if (ext == QLatin1String("PGF"))
                {
                    // Use pgf library to extract reduced version

                    PGFUtils::loadPGFScaled(qimage, path, d->storageSize());
                    failedAtPGFScaled = qimage.isNull();
                }

                if (d->observer && !d->observer->continueQuery())
                {
                    return ThumbnailImage();
                }
            }

            // Trying to load with libraw: RAW files.

            if (qimage.isNull())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to get thumbnail from Embedded preview with libraw for" << path;

                if (DRawDecoder::loadEmbeddedPreview(qimage, path))
                {
                    fromEmbeddedPreview = true;
                    profile             = metadata->getIccProfile();
                }
            }

            if (qimage.isNull())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to get thumbnail from half preview with libraw for" << path;

                // TODO: Use DImg based loader instead?

                DRawDecoder::loadHalfPreview(qimage, path);
            }

            // Special case with DNG file. See bug #338081

            if (qimage.isNull())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to get thumbnail from Embedded preview with Exiv2 for" << path;

                MetaEnginePreviews preview(path);
                qimage = preview.image();
            }

            // DImg-dependent loading methods: TIFF, PNG, everything supported by QImage

            if (qimage.isNull() && !failedAtDImg)
            {
                qimage = loadWithDImgScaled(path, &profile);

                if (d->observer && !d->observer->continueQuery())
                {
                    return ThumbnailImage();
                }
            }

            // Try JPEG anyway

            if (qimage.isNull() && !failedAtJPEGScaled)
            {
                // Use jpegutils

                JPEGUtils::loadJPEGScaled(qimage, path, d->storageSize());
            }

            // Try PGF anyway

            if (qimage.isNull() && !failedAtPGFScaled)
            {
                // Use pgfutils

                PGFUtils::loadPGFScaled(qimage, path, d->storageSize());
            }
        }
        else
        {
            // Try video thumbnail anyway

#ifdef HAVE_MEDIAPLAYER

            qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to load video preview with FFmpeg";

            VideoThumbnailer thumbnailer;
            VideoStripFilter videoStrip;

            thumbnailer.addFilter(&videoStrip);
            thumbnailer.setThumbnailSize(d->storageSize());
            thumbnailer.generateThumbnail(path, qimage);

#else

            qDebug(DIGIKAM_GENERAL_LOG) << "Cannot load video preview for" << path;
            qDebug(DIGIKAM_GENERAL_LOG) << "Video support is not available";

#endif

        }
    }

    if (qimage.isNull())
    {
        d->error = i18n("Cannot create thumbnail for %1", path);
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot create thumbnail for" << path;

        return ThumbnailImage();
    }

    qimage = scaleForStorage(qimage);

    if (colorManage && !profile.isNull())
    {
        IccManager::transformToSRGB(qimage, profile);
    }

    ThumbnailImage image;
    image.qimage          = qimage;
    image.exifOrientation = exifOrientation(info, *metadata, fromEmbeddedPreview, fromDetail);

    return image;
}

QImage ThumbnailCreator::loadWithDImgScaled(const QString& path, IccProfile* const profile) const
{
    DImg img;
    img.setAttribute(QLatin1String("scaledLoadingSize"), d->storageSize());

    if (!img.load(path, false, profile ? true : false, false, false, d->observer, d->rawSettings))
    {
        return QImage();
    }

    if (profile)
    {
        *profile = img.getIccProfile();
    }

    return img.copyQImage();
}

QImage ThumbnailCreator::loadImageDetail(const ThumbnailInfo& info,
                                         const DMetadata& metadata,
                                         const QRect& detailRect,
                                         IccProfile* const profile) const
{
    const QString& path = info.filePath;

    qDebug(DIGIKAM_GENERAL_LOG) << "Try get thumbnail from Metadata preview for" << path;

    // Check the first and largest preview (Raw files)

    MetaEnginePreviews previews(path);

    if (!previews.isEmpty())
    {
        // Discard if smaller than half preview

        QImage qimage        = previews.image();
        int acceptableWidth  = lround(previews.originalSize().width()  * 0.5);
        int acceptableHeight = lround(previews.originalSize().height() * 0.5);

        if (!qimage.isNull() && (previews.width() >= acceptableWidth) && (previews.height() >= acceptableHeight))
        {
            QSize orgSize = previews.originalSize();
            qimage        = exifRotate(qimage, exifOrientation(info, metadata, true, false));

            if (((qimage.width() < qimage.height()) && (orgSize.width() > orgSize.height())) ||
                ((qimage.width() > qimage.height()) && (orgSize.width() < orgSize.height())))
            {
                orgSize.transpose();
            }

            QRect reducedSizeDetail = TagRegion::mapFromOriginalSize(orgSize, qimage.size(), detailRect);

            return qimage.copy(reducedSizeDetail.intersected(qimage.rect()));
        }
    }

    // load DImg

    DImg img;

    DImgLoader::LoadFlags loadFlags = DImgLoader::LoadItemInfo |
                                      DImgLoader::LoadMetadata |
                                      DImgLoader::LoadICCData  |
                                      DImgLoader::LoadPreview;

    qDebug(DIGIKAM_GENERAL_LOG) << "Try to get thumbnail from DImg preview for" << path;

    if (img.load(path, loadFlags, d->observer, d->fastRawSettings))
    {
        // Discard if smaller than half preview

        unsigned int acceptableWidth  = lround(img.originalRatioSize().width()  * 0.5);
        unsigned int acceptableHeight = lround(img.originalRatioSize().height() * 0.5);

        if ((img.width() < acceptableWidth) && (img.height() < acceptableHeight))
        {
            qDebug(DIGIKAM_GENERAL_LOG) << "Preview image is smaller than the accepted size:"
                                        << acceptableWidth << "x" << acceptableHeight;
            img.reset();
        }
    }

    if (img.isNull())
    {
        qDebug(DIGIKAM_GENERAL_LOG) << "Try to get thumbnail from DImg scaled for" << path;

        // TODO: scaledLoading if detailRect is large
        // TODO: use code from PreviewTask, including cache storage

        if (!img.load(path, false, profile ? true : false, false, false, d->observer, d->fastRawSettings))
        {
            return QImage();
        }
    }

    if (profile)
    {
        *profile = img.getIccProfile();
    }

    // We must rotate before clipping because the rect refers to the oriented image.
    // I do not know currently how to back-rotate the rect for clipping before rotation.
    // If someone has the mathematics, have a go.

    img.rotateAndFlip(exifOrientation(info, metadata, false, false));

    QRect mappedDetail = TagRegion::mapFromOriginalSize(img, detailRect);
    img.crop(mappedDetail.intersected(QRect(0, 0, img.width(), img.height())));

    return img.copyQImage();
}

QImage ThumbnailCreator::loadImagePreview(const DMetadata& metadata) const
{
    QImage image;

    qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to get thumbnail with Exiv2 for" << metadata.getFilePath();

    if (metadata.getItemPreview(image))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Get thumbnail from Exif/IPTC preview. Size of image: "
                                     << image.width() << "x" << image.height();
    }
    else
    {
        // load DImg preview

        qCDebug(DIGIKAM_GENERAL_LOG) << "Trying to get thumbnail with DImg preview for" << metadata.getFilePath();

        DImg img;
        DImgLoader::LoadFlags loadFlags = DImgLoader::LoadItemInfo |
                                          DImgLoader::LoadMetadata |
                                          DImgLoader::LoadICCData  |
                                          DImgLoader::LoadPreview;

        if (img.load(metadata.getFilePath(), loadFlags, d->observer, d->fastRawSettings))
        {
            image = img.copyQImage();
        }
    }

    return image;
}

QImage ThumbnailCreator::handleAlphaChannel(const QImage& qimage) const
{
    switch (qimage.format())
    {
        case QImage::Format_RGB32:
        {
            break;
        }

        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
        {
            QImage newImage(qimage.size(), QImage::Format_RGB32);
            newImage.fill(Qt::transparent);
            QPainter p(&newImage);

            if (d->removeAlphaChannel)
            {
                QBrush brush(d->alphaImage);
                p.fillRect(newImage.rect(), brush);
            }

            p.drawImage(0, 0, qimage);
            p.end();

            return newImage;
        }

        default: // indexed and monochrome formats
        {
            return qimage.convertToFormat(QImage::Format_RGB32);
        }
    }

    return qimage;
}

int ThumbnailCreator::exifOrientation(const ThumbnailInfo& info,
                                      const DMetadata& metadata,
                                      bool fromEmbeddedPreview,
                                      bool fromDetail) const
{
    if (fromDetail)
    {
        return DMetadata::ORIENTATION_NORMAL;
    }

    return LoadSaveThread::exifOrientation(info.filePath, metadata,
                                           DImg::fileFormat(info.filePath) == DImg::RAW,
                                           fromEmbeddedPreview);
}

QImage ThumbnailCreator::exifRotate(const QImage& thumb, int orientation) const
{
    if ((orientation == DMetadata::ORIENTATION_NORMAL) ||
        (orientation == DMetadata::ORIENTATION_UNSPECIFIED))
    {
        return thumb;
    }

    QMatrix matrix = MetaEngineRotation::toMatrix((MetaEngine::ImageOrientation)orientation);

    // Transform accordingly

    return thumb.transformed(matrix);
}

} // namespace Digikam
