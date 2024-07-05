/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

// Qt
#include <QMutex>

// Self
#include "AbstractWorkerThread.h"

#include "digikam_debug.h"

namespace Marble
{

const int WAIT_ATTEMPTS = 20;
const int WAIT_TIME = 100;

class  Q_DECL_HIDDENAbstractWorkerThreadPrivate
{
 public:

    explicit AbstractWorkerThreadPrivate( AbstractWorkerThread *parent )
            : m_running( false ),
              m_end( false ),
              m_parent( parent )
    {
    }

    ~AbstractWorkerThreadPrivate()
    {
        m_end = true;
        m_parent->wait( 1000 );
    }

    QMutex m_runningMutex;
    bool m_running;
    bool m_end;

    AbstractWorkerThread *m_parent;
};


AbstractWorkerThread::AbstractWorkerThread( QObject *parent )
        : QThread( parent ),
          d( new AbstractWorkerThreadPrivate( this ) )
{
}

AbstractWorkerThread::~AbstractWorkerThread()
{
    delete d;
}

void AbstractWorkerThread::ensureRunning()
{
    QMutexLocker locker( &d->m_runningMutex );
    if ( !d->m_running ) {
        if ( wait( 2 * WAIT_TIME ) ) {
            d->m_running = true;
            start( QThread::IdlePriority );
        }
    }
}

void AbstractWorkerThread::run()
{
    int waitAttempts = WAIT_ATTEMPTS;
    while( !d->m_end ) {
        d->m_runningMutex.lock();
        if ( !workAvailable() ) {
            waitAttempts--;
            if ( !waitAttempts || d->m_end ) {
                d->m_running = false;
                d->m_runningMutex.unlock();
                break;
            }
            else {
                d->m_runningMutex.unlock();
                msleep( WAIT_TIME );
            }
        }
        else {
            d->m_runningMutex.unlock();
            work();

            waitAttempts = WAIT_ATTEMPTS;
        }
    }
}

} // namespace Marble

#include "moc_AbstractWorkerThread.cpp"
