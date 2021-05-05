/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-07-20
 * Description : Loader for thumbnails
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

#ifndef DIGIKAM_THUMB_NAIL_CREATOR_PRIVATE_H
#define DIGIKAM_THUMB_NAIL_CREATOR_PRIVATE_H

#include "thumbnailcreator.h"

// Qt includes

#include <QUrl>
#include <QFile>
#include <QBuffer>
#include <QPainter>
#include <QIODevice>
#include <QFileInfo>
#include <QUrlQuery>
#include <QApplication>
#include <QMimeDatabase>
#include <QTemporaryFile>
#include <QScopedPointer>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "metaengine_previews.h"
#include "metaengine_rotation.h"
#include "drawdecoder.h"
#include "drawfiles.h"
#include "digikam_debug.h"
#include "dimg.h"
#include "dimgloader.h"
#include "dimgloaderobserver.h"
#include "dmetadata.h"
#include "iccmanager.h"
#include "iccprofile.h"
#include "iccsettings.h"
#include "loadsavethread.h"
#include "jpegutils.h"
#include "pgfutils.h"
#include "tagregion.h"
#include "thumbsdbaccess.h"
#include "thumbsdb.h"
#include "thumbsdbbackend.h"
#include "thumbnailsize.h"

#ifdef HAVE_MEDIAPLAYER
#   include "videothumbnailer.h"
#   include "videostripfilter.h"
#endif

namespace Digikam
{

class ThumbnailImage
{
public:

    explicit ThumbnailImage()
      : exifOrientation(DMetadata::ORIENTATION_UNSPECIFIED)
    {
    }

    bool isNull() const
    {
        return qimage.isNull();
    }

public:

    QImage qimage;
    int    exifOrientation;
};

// -------------------------------------------------------------------

class Q_DECL_HIDDEN ThumbnailCreator::Private
{
public:

    explicit Private()
      : exifRotate          (true),
        removeAlphaChannel  (true),
        onlyLargeThumbnails (false),
        thumbnailStorage    (ThumbnailCreator::FreeDesktopStandard),
        infoProvider        (nullptr),
        dbIdForReplacement  (-1),
        thumbnailSize       (0),
        digiKamFingerPrint  (QLatin1String("Digikam Thumbnail Generator")), // Used internaly as PNG metadata. Do not use i18n.
        observer            (nullptr)
    {
        fastRawSettings.optimizeTimeLoading();
        fastRawSettings.rawPrm.halfSizeColorImage = true;
        fastRawSettings.rawPrm.sixteenBitsImage   = false;
    }

    bool                            exifRotate;
    bool                            removeAlphaChannel;
    bool                            onlyLargeThumbnails;

    ThumbnailCreator::StorageMethod thumbnailStorage;
    ThumbnailInfoProvider*          infoProvider;
    int                             dbIdForReplacement;

    int                             thumbnailSize;

    QString                         error;
    QString                         bigThumbPath;
    QString                         smallThumbPath;

    QString                         digiKamFingerPrint;

    QImage                          alphaImage;

    DImgLoaderObserver*             observer;
    DRawDecoding                    rawSettings;
    DRawDecoding                    fastRawSettings;

public:

    int                             storageSize() const;
};

} // namespace Digikam

#endif // DIGIKAM_THUMB_NAIL_CREATOR_PRIVATE_H
