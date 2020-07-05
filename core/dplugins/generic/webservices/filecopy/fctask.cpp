/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : file copy actions using threads.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "fctask.h"

// Qt includes

#include <QDir>
#include <QFile>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dfileoperations.h"
#include "dimg.h"
#include "previewloadthread.h"
#include "dmetadata.h"

namespace DigikamGenericFileCopyPlugin
{

class Q_DECL_HIDDEN FCTask::Private
{
public:

    explicit Private()
      : behavior(CopyFile),
        overwrite(false)
    {
    }

    QUrl srcUrl;
    QUrl dstUrl;
    int  behavior;
    bool overwrite;

    bool changeImageProperties;
    uint imageResize;
    ImageFormat imageFormat;
    uint imageCompression;
    bool removeMetadata;
};

FCTask::FCTask(const QUrl& srcUrl,
               const QUrl& dstUrl,
               int behavior,
               bool overwrite,
               bool changeImageProperties,
               uint imageResize,
               uint imageFormat,
               uint imageCompression,
               bool removeMetadata)
    : ActionJob(),
      d(new Private)
{
    d->srcUrl                = srcUrl;
    d->dstUrl                = dstUrl;
    d->behavior              = behavior;
    d->overwrite             = overwrite;

    d->changeImageProperties = changeImageProperties;
    d->imageResize           = imageResize;
    d->imageFormat           = static_cast<ImageFormat>(imageFormat);
    d->imageCompression      = imageCompression;
    d->removeMetadata        = removeMetadata;
}

FCTask::~FCTask()
{
    cancel();
    delete d;
}

void FCTask::run()
{
    if (m_cancel)
    {
        return;
    }

    QUrl dest = d->dstUrl.adjusted(QUrl::StripTrailingSlash);
    dest.setPath(dest.path() +
                 QLatin1Char('/') +
                 d->srcUrl.fileName());

    if (d->overwrite && QFile::exists(dest.toLocalFile()))
    {
        QFile::remove(dest.toLocalFile());
    }

    bool ok = false;

    if      (d->behavior == CopyFile)
    {
        if (d->changeImageProperties)
        {
            QString errString;
            ok = imageResize(d->srcUrl.toLocalFile(), dest.toLocalFile(), errString);

            if (!ok)
            {
                qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Change image error: " << errString;
            }
        }
        else
        {
            ok = QFile::copy(d->srcUrl.toLocalFile(),
                             dest.toLocalFile());
        }
    }
    else if ((d->behavior == FullSymLink) ||
             (d->behavior == RelativeSymLink))
    {
#ifdef Q_OS_WIN
        dest.setPath(dest.path() + QLatin1String(".lnk"));
#endif

        if (d->behavior == FullSymLink)
        {
            ok = QFile::link(d->srcUrl.toLocalFile(),
                             dest.toLocalFile());
        }
        else
        {
            QDir dir(d->dstUrl.toLocalFile());
            QString path = dir.relativeFilePath(d->srcUrl.toLocalFile());
            QUrl srcUrl  = QUrl::fromLocalFile(path);
            ok           = QFile::link(srcUrl.toLocalFile(),
                                       dest.toLocalFile());
        }
    }

    if (ok && (d->behavior == CopyFile))
    {
        DFileOperations::copyModificationTime(d->srcUrl.toLocalFile(),
                                              dest.toLocalFile());
    }

    if (ok)
    {
        emit signalUrlProcessed(d->srcUrl, dest);
    }

    emit signalDone();
}

bool FCTask::imageResize(const QString& orgUrl, const QString& destName, QString& err)
{
    QFileInfo fi(orgUrl);

    if (!fi.exists() || !fi.isReadable())
    {
        err = i18n("Error opening input file");
        return false;
    }

    QFileInfo destInfo(destName);
    QFileInfo tmpDir(destInfo.dir().absolutePath());

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "tmpDir: " << destInfo.dir().absolutePath();

    if (!tmpDir.exists() || !tmpDir.isWritable())
    {
        err = i18n("Error opening temporary folder");
        return false;
    }

    DImg img = PreviewLoadThread::loadFastSynchronously(orgUrl, d->imageResize);

    if (img.isNull())
    {
        img.load(orgUrl);
    }

    uint sizeFactor = d->imageResize;

    if (!img.isNull())
    {
        uint w = img.width();
        uint h = img.height();

        if ((w > sizeFactor) || (h > sizeFactor))
        {
            if (w > h)
            {
                h = (uint)((double)(h * sizeFactor) / w);

                if (h == 0)
                {
                    h = 1;
                }

                w = sizeFactor;

                Q_ASSERT(h <= sizeFactor);
            }
            else
            {
                w = (uint)((double)(w * sizeFactor) / h);

                if (w == 0)
                {
                    w = 1;
                }

                h = sizeFactor;

                Q_ASSERT(w <= sizeFactor);
            }

            DImg scaledImg = img.smoothScale(w, h, Qt::IgnoreAspectRatio);

            if ((scaledImg.width() != w) || (scaledImg.height() != h))
            {
                err = i18n("Cannot resize image. Aborting.");
                return false;
            }

            img = scaledImg;
        }

        QString destFile = destInfo.path()  +
                           QLatin1Char('/') +
                           destInfo.completeBaseName();

        if (d->imageFormat == ImageFormat::JPEG)
        {
            destFile.append(QLatin1String(".jpeg"));

            img.setAttribute(QLatin1String("quality"), d->imageCompression);

            if (!img.save(destFile, QLatin1String("JPEG")))
            {
                err = i18n("Cannot save resized image (JPEG). Aborting.");
                return false;
            }
        }
        else if (d->imageFormat == ImageFormat::PNG)
        {
            destFile.append(QLatin1String(".png"));

            if (!img.save(destFile, QLatin1String("PNG")))
            {
                err = i18n("Cannot save resized image (PNG). Aborting.");
                return false;
            }
        }

        DMetadata meta;

        if (!meta.load(destFile))
        {
            return false;
        }

        if (d->removeMetadata)
        {
            meta.clearExif();
            meta.clearIptc();
            meta.clearXmp();
        }
        else
        {
            meta.setItemOrientation(MetaEngine::ORIENTATION_NORMAL);
        }

        meta.setMetadataWritingMode((int)DMetadata::WRITE_TO_FILE_ONLY);

        if (!meta.save(destFile))
        {
            return false;
        }

        return true;
    }

    return false;
}

} // namespace DigikamGenericFileCopyPlugin
