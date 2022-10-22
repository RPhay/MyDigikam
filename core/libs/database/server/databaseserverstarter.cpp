/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-08
 * Description : database server starter
 *
 * SPDX-FileCopyrightText: 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * SPDX-FileCopyrightText: 2016      by Swati Lodha <swatilodha27 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "databaseserverstarter.h"

// Qt includes

#include <QList>
#include <QStringList>
#include <QtGlobal>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QVariant>
#include <QSystemSemaphore>
#include <QApplication>
#include <QCoreApplication>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "databaseserver.h"

namespace Digikam
{

class Q_DECL_HIDDEN DatabaseServerStarter::Private
{
public:

    explicit Private()
        : internalServer(nullptr)
    {
    }

    DatabaseServer* internalServer;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DatabaseServerStarterCreator
{
public:

    DatabaseServerStarter object;
};

Q_GLOBAL_STATIC(DatabaseServerStarterCreator, databaseServerStarterCreator)

// -----------------------------------------------------------------------------------------------

DatabaseServerStarter::DatabaseServerStarter()
    : d(new Private)
{
}

DatabaseServerStarter::~DatabaseServerStarter()
{
    delete d;
}

DatabaseServerStarter* DatabaseServerStarter::instance()
{
    return &databaseServerStarterCreator->object;
}

DatabaseServerError DatabaseServerStarter::startServerManagerProcess(const DbEngineParameters& parameters) const
{
    DatabaseServerError result;

    d->internalServer = new DatabaseServer(parameters);

    QSystemSemaphore sem(QLatin1String("DigikamDBSrvAccess"), 1, QSystemSemaphore::Open);
    sem.acquire();

    result            = d->internalServer->startDatabaseProcess();

    if (result.getErrorType() != DatabaseServerError::NoErrors)
    {
        qCDebug(DIGIKAM_DATABASESERVER_LOG) << "Cannot start internal database server";
    }
    else
    {
        qCDebug(DIGIKAM_DATABASESERVER_LOG) << "Internal database server started";
        d->internalServer->start();
    }

    sem.release();

    return result;
}

void DatabaseServerStarter::stopServerManagerProcess()
{
    if (!d->internalServer)
    {
        return;
    }

    QSystemSemaphore sem(QLatin1String("DigikamDBSrvAccess"), 1, QSystemSemaphore::Open);
    sem.acquire();

    d->internalServer->stopDatabaseProcess();
    d->internalServer = nullptr;

    sem.release();

    qCDebug(DIGIKAM_DATABASESERVER_LOG) << "Internal database server stopped";
}

} // namespace Digikam
