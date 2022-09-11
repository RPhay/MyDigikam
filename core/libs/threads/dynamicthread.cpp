/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-13
 * Description : Dynamically active thread
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dynamicthread.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

// Local includes

#include "digikam_debug.h"
#include "threadmanager.h"

namespace Digikam
{

class Q_DECL_HIDDEN DynamicThread::Private : public QRunnable
{
public:

    explicit Private(DynamicThread* const q)
        : q                 (q),
          assignedThread    (nullptr),
          running           (true),
          emitSignals       (false),
          inDestruction     (false),
          threadRequested   (false),
          state             (DynamicThread::Inactive),
          priority          (QThread::InheritPriority),
          previousPriority  (QThread::InheritPriority)
    {
        setAutoDelete(false);
    };

    void run() override;

    void         takingThread();
    bool         transitionToRunning();
    void         transitionToInactive();

public:

    DynamicThread* const          q;
    QThread*                      assignedThread;

    volatile bool                 running;
    volatile bool                 emitSignals;
    volatile bool                 inDestruction;
    volatile bool                 threadRequested;

    volatile DynamicThread::State state;

    QThread::Priority             priority;
    QThread::Priority             previousPriority;

    QMutex                        mutex;
    QWaitCondition                condVar;
};

void DynamicThread::Private::takingThread()
{
    QMutexLocker locker(&mutex);

    // The thread we requested from the pool has now "arrived"

    threadRequested = false;
}

bool DynamicThread::Private::transitionToRunning()
{
    QMutexLocker locker(&mutex);

    switch (state)
    {
        case DynamicThread::Scheduled:
        {
            // ensure that a newly scheduled thread does not run
            // while an old, deactivated one has not yet called transitionToInactive

            while (assignedThread)
            {
                condVar.wait(&mutex);
            }

            state            = DynamicThread::Running;
            running          = true;
            assignedThread   = QThread::currentThread();
            previousPriority = assignedThread->priority();

            if (priority != QThread::InheritPriority)
            {
                assignedThread->setPriority(priority);
            }

            return true;
        }

        case DynamicThread::Deactivating:
        {
            return false;
        }

        case DynamicThread::Running:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Transition to Running: Invalid Running state" << q;
            return false;
        }

        case DynamicThread::Inactive:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Transition to Running: Invalid Inactive state" << q;
            return false;
        }

        default:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Transition to Running: Should never reach here: assert?" << q;
            return false;
        }
    }
}

void DynamicThread::Private::transitionToInactive()
{
    QMutexLocker locker(&mutex);

    switch (state)
    {
        case DynamicThread::Scheduled:
        case DynamicThread::Deactivating:
        case DynamicThread::Running:
        {
            if (previousPriority != QThread::InheritPriority)
            {
                assignedThread->setPriority(previousPriority);
                previousPriority = QThread::InheritPriority;
            }

            assignedThread = nullptr;

            if (state != DynamicThread::Scheduled)
            {
                state = DynamicThread::Inactive;
            }

            condVar.wakeAll();
            break;
        }

        case DynamicThread::Inactive:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Transition to Inactive: Invalid Inactive state" << q;
            break;
        }
    }
}

void DynamicThread::Private::run()
{
    if (emitSignals)
    {
        Q_EMIT q->starting();
    }

    if (transitionToRunning())
    {
        takingThread();
        q->run();
    }
    else
    {
        takingThread();
    }

    if (emitSignals)
    {
        Q_EMIT q->finished();
    }

    transitionToInactive();

    // As soon as we are inactive, we may get deleted!
}

// -----------------------------------------------------------------------------------------------

DynamicThread::DynamicThread(QObject* const parent)
    : QObject(parent),
      d      (new Private(this))
{
    ThreadManager::instance()->initialize(this);
}

DynamicThread::~DynamicThread()
{
    shutDown();
    delete d;
}

void DynamicThread::shutDown()
{
    QMutexLocker locker(&d->mutex);
    d->running       = false;
    d->inDestruction = true;
    stop(locker);
    wait(locker);
}

DynamicThread::State DynamicThread::state() const
{
    return (d->state);
}

bool DynamicThread::isRunning() const
{
    return ((d->state == Scheduled) || (d->state == Running) || (d->state == Deactivating));
}

QMutex* DynamicThread::threadMutex() const
{
    return (&d->mutex);
}

bool DynamicThread::isFinished() const
{
    return (d->state == Inactive);
}

void DynamicThread::setEmitSignals(bool emitThem)
{
    d->emitSignals = emitThem;
}

void DynamicThread::setPriority(QThread::Priority priority)
{
    if (d->priority == priority)
    {
        return;
    }

    d->priority = priority;

    if (d->priority != QThread::InheritPriority)
    {
        QMutexLocker locker(&d->mutex);

        if (d->assignedThread)
        {
            d->assignedThread->setPriority(d->priority);
        }
    }
}

QThread::Priority DynamicThread::priority() const
{
    return (d->priority);
}

void DynamicThread::start()
{
    QMutexLocker locker(&d->mutex);
    start(locker);
}

void DynamicThread::stop()
{
    QMutexLocker locker(&d->mutex);
    stop(locker);
}

void DynamicThread::wait()
{
    QMutexLocker locker(&d->mutex);
    wait(locker);
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
void DynamicThread::start(QMutexLocker<QMutex>& locker)
#else
void DynamicThread::start(QMutexLocker& locker)
#endif
{
    if (d->inDestruction)
    {
        return;
    }

    switch (d->state)
    {
        case Inactive:
        case Deactivating:
        {
            d->state = Scheduled;
            break;
        }

        case Running:
        case Scheduled:
        {
            return;
        }
    }

    if (!d->threadRequested)
    {
        // Avoid issuing multiple thread requests after very fast start/stop/start calls

        d->threadRequested = true;

        locker.unlock();
        ThreadManager::instance()->schedule(d);
        locker.relock();
    }
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
void DynamicThread::stop(QMutexLocker<QMutex>& locker)
#else
void DynamicThread::stop(QMutexLocker& locker)
#endif
{
    Q_UNUSED(locker);

    switch (d->state)
    {
        case Scheduled:
        case Running:
        {
            d->running = false;
            d->state   = Deactivating;
            break;
        }

        case Inactive:
        case Deactivating:
        {
            break;
        }
    }
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
void DynamicThread::wait(QMutexLocker<QMutex>& locker)
#else
void DynamicThread::wait(QMutexLocker& locker)
#endif
{
    while (d->state != Inactive)
    {
        d->condVar.wait(locker.mutex());
    }
}

bool DynamicThread::runningFlag() const volatile
{
    return (d->running);
}

} // namespace Digikam
