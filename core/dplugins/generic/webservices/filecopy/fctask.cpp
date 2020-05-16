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

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dfileoperations.h"

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

};

FCTask::FCTask(const QUrl& srcUrl,
               const QUrl& dstUrl,
               int behavior, bool overwrite)
    : ActionJob(),
      d(new Private)
{
    d->srcUrl    = srcUrl;
    d->dstUrl    = dstUrl;
    d->behavior  = behavior;
    d->overwrite = overwrite;

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
        ok = QFile::copy(d->srcUrl.toLocalFile(),
                         dest.toLocalFile());
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

} // namespace DigikamGenericFileCopyPlugin
