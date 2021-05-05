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

#include "vkontakte_allnoteslistjob.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Vkontakte
{

class AllNotesListJob::Private
{
public:

    QString            accessToken;
    int                uid;
    int                totalCount;
    QList<NoteInfoPtr> list;
};

AllNotesListJob::AllNotesListJob(const QString& accessToken, int uid)
    : KJobWithSubjobs(),
      d(new Private)
{
    d->accessToken = accessToken;
    d->uid         = uid;
    d->totalCount  = -1;
}

AllNotesListJob::~AllNotesListJob()
{
    delete d;
}

void AllNotesListJob::startNewJob(int offset, int count)
{
    NotesListJob* const job = new NotesListJob(d->accessToken, d->uid, offset, count);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(jobFinished(KJob*)));

    m_jobs.append(job);
    job->start();
}

void AllNotesListJob::start()
{
    startNewJob(0, 100);
}

void AllNotesListJob::jobFinished(KJob *kjob)
{
    NotesListJob* const job = dynamic_cast<NotesListJob*>(kjob);

    Q_ASSERT(job);

    if (!job)
    {
        return;
    }

    m_jobs.removeAll(job);

    if (job->error())
    {
        setError(job->error());
        setErrorText(job->errorText());
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Job error: " << job->errorString();

        return;
    }

    d->list.append(job->list());

    // If this was the first job, start all others

    if      (d->totalCount == -1)
    {
        d->totalCount = job->totalCount();

        for (int offset = 100; offset < d->totalCount; offset += 100)
        {
            startNewJob(offset, qMin(100, d->totalCount - offset));
        }
    }
    else if (d->totalCount != job->totalCount())
    {
        // TODO: some new notes might have been added, what should we do then?

        doKill();
        setError(KJob::UserDefinedError + 1);
        setErrorText(i18n("The number of notes has changed between requests."));
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Job error: " << job->errorString();
        emitResult();

        return;
    }

    // All jobs have finished

    if (m_jobs.size() == 0)
    {
//        qSort(list); // sort by message ID (which should be equivalent to sorting by date)
        emitResult();
    }
}

QList<NoteInfoPtr> AllNotesListJob::list() const
{
    return d->list;
}

int AllNotesListJob::count() const
{
    return d->list.size();
}

} // namespace Vkontakte
