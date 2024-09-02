/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : Low level threads management for batch processing on multi-core
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2014 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * SPDX-FileCopyrightText: 2011-2012 by Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "actionthreadbase.h"

// Qt includes

#include <QMutexLocker>
#include <QWaitCondition>
#include <QMutex>
#include <QThreadPool>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

ActionJob::ActionJob(QObject* const parent)
    : QObject  (parent),
      QRunnable()
{
    setAutoDelete(false);
}

ActionJob::~ActionJob()
{
    cancel();
}

void ActionJob::cancel()
{
    m_cancel = true;
}

// -----------------------------------------------------------------

class Q_DECL_HIDDEN ActionThreadBase::Private
{
public:

    Private() = default;

    volatile bool       running = false;

    QWaitCondition      condVarJobs;
    QMutex              mutex;

    ActionJobCollection todo;
    ActionJobCollection pending;
    ActionJobCollection processed;

    QThreadPool*        pool    = nullptr;
};

ActionThreadBase::ActionThreadBase(QObject* const parent)
    : QThread(parent),
      d      (new Private)
{
    d->pool = new QThreadPool(this);

    setDefaultMaximumNumberOfThreads();
}

ActionThreadBase::~ActionThreadBase()
{
    // Cancel the thread

    cancel(false);

    // Wait for the thread to finish

    wait();

    // Wait for the jobs to finish

    d->pool->waitForDone();

    // Cleanup all jobs from memory

    const auto keys = d->processed.keys();

    for (ActionJob* const job : keys)
    {
        delete job;
    }

    delete d;
}

void ActionThreadBase::setMaximumNumberOfThreads(int n)
{
    d->pool->setMaxThreadCount(n);

    qCDebug(DIGIKAM_GENERAL_LOG) << "Using " << n << " CPU core to run threads";
}

int ActionThreadBase::maximumNumberOfThreads() const
{
    return d->pool->maxThreadCount();
}

void ActionThreadBase::setDefaultMaximumNumberOfThreads()
{
    setMaximumNumberOfThreads(QThread::idealThreadCount());
}

void ActionThreadBase::slotJobFinished()
{
    ActionJob* const job = dynamic_cast<ActionJob*>(sender());

    if (!job)
    {
        return;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "One job is done " << job
                                 << " time:" << job->m_timer.elapsed();

    QMutexLocker lock(&d->mutex);

    d->processed.insert(job, 0);
    d->pending.remove(job);

    if (isEmpty())
    {
        d->running = false;
    }

    d->condVarJobs.wakeAll();
}

void ActionThreadBase::cancel(bool isCancel)
{
    if (isCancel)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cancel Main Thread";
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Finish Main Thread";
    }

    QMutexLocker lock(&d->mutex);

    const auto keys  = d->todo.keys();

    for (ActionJob* const job : keys)
    {
        delete job;
    }

    const auto keys2 = d->pending.keys();

    for (ActionJob* const job : keys2)
    {
        job->cancel();
        d->processed.insert(job, 0);
    }

    d->todo.clear();
    d->pending.clear();
    d->running = false;

    d->condVarJobs.wakeAll();
}

bool ActionThreadBase::isEmpty() const
{
    return (d->pending.isEmpty());
}

int ActionThreadBase::pendingCount() const
{
    return (d->pending.count());
}

void ActionThreadBase::appendJobs(const ActionJobCollection& jobs)
{
    QMutexLocker lock(&d->mutex);

    for (ActionJobCollection::const_iterator it = jobs.begin() ; it != jobs.end() ; ++it)
    {
        d->todo.insert(it.key(), it.value());
    }

    d->condVarJobs.wakeAll();
}

void ActionThreadBase::run()
{
    d->running = true;

    while (d->running)
    {
        QMutexLocker lock(&d->mutex);

        if (!d->todo.isEmpty())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Action Thread run" << d->todo.count() << "new jobs";

            for (ActionJobCollection::iterator it = d->todo.begin() ; it != d->todo.end() ; ++it)
            {
                ActionJob* const job = it.key();
                int priority         = it.value();

                connect(job, SIGNAL(signalDone()),
                        this, SLOT(slotJobFinished()));

                job->m_timer.start();
                d->pool->start(job, priority);
                d->pending.insert(job, priority);
            }

            d->todo.clear();
        }
        else
        {
            d->condVarJobs.wait(&d->mutex);
        }
    }
}

} // namespace Digikam

#include "moc_actionthreadbase.cpp"
