/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : file copy actions using threads.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Maik Qualmann <metzpinguin at gmail dot com>
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
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dfileoperations.h"
#include "previewloadthread.h"
#include "dmetadata.h"
#include "dimg.h"

namespace DigikamGenericFileCopyPlugin
{

class Q_DECL_HIDDEN FCTask::Private
{
public:

    explicit Private()
    {
    }

    QUrl        srcUrl;
    FCContainer settings;
};

FCTask::FCTask(const QUrl& srcUrl,
               const FCContainer& settings)
    : ActionJob(),
      d        (new Private)
{
    d->srcUrl   = srcUrl;
    d->settings = settings;
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

    QUrl dest = d->settings.destUrl.adjusted(QUrl::StripTrailingSlash);
    dest.setPath(dest.path() +
                 QLatin1Char('/') +
                 d->srcUrl.fileName());

    bool ok = false;

    if      (d->settings.behavior == FCContainer::CopyFile)
    {
        QFileInfo srcInfo(d->srcUrl.toLocalFile());
        QString suffix = srcInfo.suffix().toUpper();

        QMimeDatabase mimeDB;
        QString mimeType(mimeDB.mimeTypeForUrl(d->srcUrl).name());

        if (d->settings.changeImageProperties             &&
            (
             mimeType.startsWith(QLatin1String("image/")) ||
             (suffix == QLatin1String("PGF"))             ||
             (suffix == QLatin1String("KRA"))             ||
             (suffix == QLatin1String("HEIC"))            ||
             (suffix == QLatin1String("HEIF"))
            )
           )
        {
            ok = imageResize(d->srcUrl.toLocalFile(), dest.toLocalFile());
        }
        else
        {
            deleteTargetFile(dest.toLocalFile());
            ok = QFile::copy(d->srcUrl.toLocalFile(),
                             dest.toLocalFile());
            if (ok)
            {
                DFileOperations::copyModificationTime(d->srcUrl.toLocalFile(),
                                                      dest.toLocalFile());
            }
        }
    }
    else if ((d->settings.behavior == FCContainer::FullSymLink) ||
             (d->settings.behavior == FCContainer::RelativeSymLink))
    {

#ifdef Q_OS_WIN

        dest.setPath(dest.path() + QLatin1String(".lnk"));

#endif

        if (d->settings.behavior == FCContainer::FullSymLink)
        {
            deleteTargetFile(dest.toLocalFile());
            ok = QFile::link(d->srcUrl.toLocalFile(),
                             dest.toLocalFile());
        }
        else
        {
            QDir dir(d->settings.destUrl.toLocalFile());
            QString path = dir.relativeFilePath(d->srcUrl.toLocalFile());
            QUrl srcUrl  = QUrl::fromLocalFile(path);

            deleteTargetFile(dest.toLocalFile());
            ok           = QFile::link(srcUrl.toLocalFile(),
                                       dest.toLocalFile());
        }
    }

    if (ok)
    {
        emit signalUrlProcessed(d->srcUrl, dest);
    }

    emit signalDone();
}

bool FCTask::imageResize(const QString& orgPath, const QString& destPath)
{
    QFileInfo fi(orgPath);

    if (!fi.exists() || !fi.isReadable())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Error opening input file"
                                         << fi.filePath();
        return false;
    }

    QFileInfo destInfo(destPath);
    QFileInfo tmpDir(destInfo.dir().absolutePath());

    if (!tmpDir.exists() || !tmpDir.isWritable())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Error opening target folder"
                                         << tmpDir.dir();
        return false;
    }

    DImg img = PreviewLoadThread::loadFastSynchronously(orgPath, d->settings.imageResize);

    if (img.isNull())
    {
        img.load(orgPath);
    }

    if (!img.isNull())
    {
        uint sizeFactor = d->settings.imageResize;

        if ((img.width() > sizeFactor) || (img.height() > sizeFactor))
        {
            DImg scaledImg = img.smoothScale(sizeFactor,
                                             sizeFactor,
                                             Qt::KeepAspectRatio);

            if ((scaledImg.width() > sizeFactor) || (scaledImg.height() > sizeFactor))
            {
                qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Cannot resize image";
                return false;
            }

            img = scaledImg;
        }

        QString destFile = destInfo.path()  +
                           QLatin1Char('/') +
                           destInfo.completeBaseName();

        if (d->settings.imageFormat == FCContainer::JPEG)
        {
            destFile.append(QLatin1String(".jpeg"));
            deleteTargetFile(destFile);

            img.setAttribute(QLatin1String("quality"), d->settings.imageCompression);

            if (!img.save(destFile, QLatin1String("JPEG")))
            {
                qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Cannot save resized image (JPEG)";
                return false;
            }
        }
        else if (d->settings.imageFormat == FCContainer::PNG)
        {
            destFile.append(QLatin1String(".png"));
            deleteTargetFile(destFile);

            if (!img.save(destFile, QLatin1String("PNG")))
            {
                qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Cannot save resized image (PNG)";
                return false;
            }
        }

        QScopedPointer<DMetadata> meta(new DMetadata);

        if (!meta->load(destFile))
        {
            return false;
        }

        if (d->settings.removeMetadata)
        {
            meta->clearExif();
            meta->clearIptc();
            meta->clearXmp();
        }
        else
        {
            meta->setItemOrientation(MetaEngine::ORIENTATION_NORMAL);
        }

        meta->setMetadataWritingMode((int)DMetadata::WRITE_TO_FILE_ONLY);

        if (!meta->save(destFile))
        {
            return false;
        }

        DFileOperations::copyModificationTime(orgPath, destFile);

        return true;
    }

    return false;
}

void FCTask::deleteTargetFile(const QString& filePath)
{
    if (d->settings.overwrite && QFile::exists(filePath))
    {
        QFile::remove(filePath);
    }
}

} // namespace DigikamGenericFileCopyPlugin
