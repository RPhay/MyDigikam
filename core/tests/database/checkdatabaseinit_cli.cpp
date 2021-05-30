/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-21
 * Description : CLI test program for digiKam DB init
 *
 * Copyright (C) 2014-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QString>
#include <QTimer>
#include <QTest>
#include <QCommandLineParser>

// KDE includes

#include <kaboutdata.h>

// Local includes

#include "digikam_debug.h"
#include "daboutdata.h"
#include "albummanager.h"
#include "coredbaccess.h"
#include "thumbsdbaccess.h"
#include "facedbaccess.h"
#include "dbengineparameters.h"
#include "scancontroller.h"
#include "digikam_version.h"

using namespace Digikam;

const QString IMAGE_PATH(QFINDTESTDATA("data/testimages/"));

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "testdatabaseinit - test database initialization";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <dbtype: sqlite | mysql>";
        return -1;
    }

    KAboutData aboutData(QLatin1String("digikam"),
                         QLatin1String("digiKam"), // No need i18n here.
                         digiKamVersion());

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    qCDebug(DIGIKAM_TESTS_LOG) << "Setup Database...";
    DbEngineParameters params;
    QString dbtype = QLatin1String(argv[1]);

    // ------------------------------------------------------------------------------------

    if (dbtype == QLatin1String("sqlite"))
    {
        params.databaseType = DbEngineParameters::SQLiteDatabaseType();
        params.setCoreDatabasePath(QDir::currentPath() + QLatin1String("/digikam-core-test.db"));
        params.setThumbsDatabasePath(QDir::currentPath() + QLatin1String("/digikam-thumbs-test.db"));
        params.setFaceDatabasePath(QDir::currentPath() + QLatin1String("/digikam-faces-test.db"));
        params.legacyAndDefaultChecks();
    }
    else if (dbtype == QLatin1String("mysql"))
    {
        QString defaultAkDir              = DbEngineParameters::internalServerPrivatePath();
        QString miscDir                   = QDir(defaultAkDir).absoluteFilePath(QLatin1String("db_misc"));
        params.databaseType               = DbEngineParameters::MySQLDatabaseType();
        params.databaseNameCore           = QLatin1String("digikam");
        params.databaseNameThumbnails     = QLatin1String("digikam");
        params.databaseNameFace           = QLatin1String("digikam");
        params.userName                   = QLatin1String("root");
        params.password                   = QString();
        params.internalServer             = true;
        params.internalServerDBPath       = QDir::currentPath();
        params.internalServerMysqlServCmd = DbEngineParameters::defaultMysqlServerCmd();
        params.internalServerMysqlInitCmd = DbEngineParameters::defaultMysqlInitCmd();
        params.hostName                   = QString();
        params.port                       = -1;
        params.connectOptions             = QString::fromLatin1("UNIX_SOCKET=%1/mysql.socket").arg(miscDir);
    }
    else
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Wrong database type to use: " << dbtype;
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <dbtype: sqlite | mysql>";
        return -1;
    }

    // ------------------------------------------------------------------------------------

    qCDebug(DIGIKAM_TESTS_LOG) << "Initializing database...";
    bool b = AlbumManager::instance()->setDatabase(params, false, IMAGE_PATH);

    qCDebug(DIGIKAM_TESTS_LOG) << "Database initialization done: " << b;

    QTest::qWait(3000);

    qCDebug(DIGIKAM_TESTS_LOG) << "Shutting down database";
    ScanController::instance()->shutDown();
    AlbumManager::instance()->cleanUp();

    qCDebug(DIGIKAM_TESTS_LOG) << "Cleaning DB now";
    CoreDbAccess::cleanUpDatabase();
    ThumbsDbAccess::cleanUpDatabase();
    FaceDbAccess::cleanUpDatabase();

    return 0;
}
