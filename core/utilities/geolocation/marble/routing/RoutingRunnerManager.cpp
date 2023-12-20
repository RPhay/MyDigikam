// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "RoutingRunnerManager.h"

#include <QThreadPool>
#include <QTimer>

#include "MarblePlacemarkModel.h"
#include "MarbleModel.h"
#include "Planet.h"
#include "GeoDataDocument.h"
#include "PluginManager.h"
#include "RoutingRunnerPlugin.h"
#include "RunnerTask.h"
#include "RouteRequest.h"
#include "RoutingProfilesModel.h"
#include "RoutingRunner.h"

#include "digikam_debug.h"

namespace Marble
{

class MarbleModel;

RoutingTask::RoutingTask( RoutingRunner *runner, RoutingRunnerManager *manager, const RouteRequest* routeRequest ) :
    QObject(),
    m_runner( runner ),
    m_routeRequest( routeRequest )
{
    connect( m_runner, SIGNAL(routeCalculated(GeoDataDocument*)),
             manager, SLOT(addRoutingResult(GeoDataDocument*)) );
}

void RoutingTask::run()
{
    m_runner->retrieveRoute( m_routeRequest );
    m_runner->deleteLater();

    Q_EMIT finished( this );
}

class Q_DECL_HIDDEN RoutingRunnerManager::Private
{
public:
    Private( RoutingRunnerManager *parent, const MarbleModel *marbleModel );

    ~Private();

    template<typename T>
    QList<T*> plugins( const QList<T*> &plugins ) const;

    void addRoutingResult( GeoDataDocument *route );
    void cleanupRoutingTask( RoutingTask *task );

    RoutingRunnerManager *const q;
    const MarbleModel *const m_marbleModel;
    const PluginManager *const m_pluginManager;
    QList<RoutingTask*> m_routingTasks;
    QVector<GeoDataDocument*> m_routingResult;
};

RoutingRunnerManager::Private::Private( RoutingRunnerManager *parent, const MarbleModel *marbleModel ) :
    q( parent ),
    m_marbleModel( marbleModel ),
    m_pluginManager( marbleModel->pluginManager() )
{
    qRegisterMetaType<GeoDataDocument*>( "GeoDataDocument*" );
}

RoutingRunnerManager::Private::~Private()
{
    // nothing to do
}

template<typename T>
QList<T*> RoutingRunnerManager::Private::plugins( const QList<T*> &plugins ) const
{
    QList<T*> result;
    for( T* plugin: plugins ) {
        if ( ( m_marbleModel && m_marbleModel->workOffline() && !plugin->canWorkOffline() ) ) {
            continue;
        }

        if ( !plugin->canWork() ) {
            continue;
        }

        if ( m_marbleModel && !plugin->supportsCelestialBody( m_marbleModel->planet()->id() ) )
        {
            continue;
        }

        result << plugin;
    }

    return result;
}

void RoutingRunnerManager::Private::addRoutingResult( GeoDataDocument *route )
{
    if ( route ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "route retrieved";
        m_routingResult.push_back( route );
        Q_EMIT q->routeRetrieved( route );
    }
}

void RoutingRunnerManager::Private::cleanupRoutingTask( RoutingTask *task )
{
    m_routingTasks.removeAll( task );
    qCDebug(DIGIKAM_MARBLE_LOG) << "removing task" << m_routingTasks.size() << " " << (quintptr)task;
    if ( m_routingTasks.isEmpty() ) {
        if ( m_routingResult.isEmpty() ) {
            Q_EMIT q->routeRetrieved( nullptr );
        }

        Q_EMIT q->routingFinished();
    }
}

RoutingRunnerManager::RoutingRunnerManager( const MarbleModel *marbleModel, QObject *parent )
    : QObject( parent ),
      d( new Private( this, marbleModel ) )
{
    if ( QThreadPool::globalInstance()->maxThreadCount() < 4 ) {
        QThreadPool::globalInstance()->setMaxThreadCount( 4 );
    }
}

RoutingRunnerManager::~RoutingRunnerManager()
{
    delete d;
}

void RoutingRunnerManager::retrieveRoute( const RouteRequest *request )
{
    RoutingProfile profile = request->routingProfile();

    d->m_routingTasks.clear();
    d->m_routingResult.clear();

    QList<RoutingRunnerPlugin*> plugins;// = d->plugins( d->m_pluginManager->routingRunnerPlugins() );
    for( RoutingRunnerPlugin* plugin: plugins ) {
        if ( !profile.name().isEmpty() && !profile.pluginSettings().contains( plugin->nameId() ) ) {
            continue;
        }

        RoutingTask* task = new RoutingTask( plugin->newRunner(), this, request );
        connect( task, SIGNAL(finished(RoutingTask*)), this, SLOT(cleanupRoutingTask(RoutingTask*)) );
        qCDebug(DIGIKAM_MARBLE_LOG) << "route task" << plugin->nameId() << " " << (quintptr)task;
        d->m_routingTasks << task;
    }

    for( RoutingTask* task: d->m_routingTasks ) {
        QThreadPool::globalInstance()->start( task );
    }

    if ( d->m_routingTasks.isEmpty() ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "No suitable routing plugins found, cannot retrieve a route";
        d->cleanupRoutingTask( nullptr );
    }
}

QVector<GeoDataDocument*> RoutingRunnerManager::searchRoute( const RouteRequest *request, int timeout ) {
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(routingFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( timeout );
    retrieveRoute( request );
    localEventLoop.exec();
    return d->m_routingResult;
}

}

#include "moc_RoutingRunnerManager.cpp"
