/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "vkontakte_photojob.h"

// Qt includes

#include <QImage>

// KDE includes

#include <KIO/Job>

// Local includes

#include "digikam_debug.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN PhotoJob::Private
{
public:

    QUrl   url;
    QImage photo;
};

PhotoJob::PhotoJob(const QUrl& url)
    : d(new Private)
{
    d->url = url;
}

PhotoJob::~PhotoJob()
{
    delete d;
}

void PhotoJob::start()
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Starting photo download" << d->url;
    KIO::StoredTransferJob* const job = KIO::storedGet(d->url, KIO::Reload, KIO::HideProgressInfo);
    m_job                             = job;

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(jobFinished(KJob*)));

    job->start();
}

void PhotoJob::jobFinished(KJob* kjob)
{
    KIO::StoredTransferJob* const job = dynamic_cast<KIO::StoredTransferJob*>(kjob);

    if (!job)
    {
        return;
    }

    if (job->error())
    {
        setError(job->error());
        setErrorText(KIO::buildErrorString(error(), job->errorText()));
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Job error:" << job->errorString();
    }
    else
    {
        d->photo = QImage::fromData(job->data());
    }

    emitResult();
    m_job = nullptr;
}

QImage PhotoJob::photo() const
{
    return d->photo;
}

} // namespace Vkontakte
