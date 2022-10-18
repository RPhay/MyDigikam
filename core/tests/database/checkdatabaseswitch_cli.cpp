/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-21
 * Description : CLI test program for switch digiKam DB from sqlite to mysql
 *
 * SPDX-FileCopyrightText: 2014-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
#include "dtestdatadir.h"
#include "wstoolutils.h"
#include "mysqladminbinary.h"
#include "mysqlinitbinary.h"
#include "mysqlservbinary.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "testdatabaseswitch - test database switch";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <sqliteToMysql | mysqlToSqlite>";
        return -1;
    }


    MysqlInitBinary  mysqlInitBin;

    if (!mysqlInitBin.recheckDirectories())
    {
        qCWarning(DIGIKAM_TESTS_LOG) << "Not able to found the Mysql Init binary program. Test is aborted...";
        return -1;
    }

    MysqlAdminBinary mysqlAdminBin;

    if (!mysqlAdminBin.recheckDirectories())
    {
        qCWarning(DIGIKAM_TESTS_LOG) << "Not able to found the Mysql Admin binary program. Test is aborted...";
        return -1;
    }

    MysqlServBinary  mysqlServBin;

    if (!mysqlServBin.recheckDirectories())
    {
        qCWarning(DIGIKAM_TESTS_LOG) << "Not able to found the Mysql Server binary program. Test is aborted...";
        return -1;
    }

    QString tempPath = QString::fromLatin1(QTest::currentAppName());
    tempPath.replace(QLatin1String("./"), QString());
    QDir tempDir     = WSToolUtils::makeTemporaryDir(tempPath.toLatin1().data());
    qCDebug(DIGIKAM_TESTS_LOG) << "Database Dir:" << tempDir.path();

    QString filesPath = DTestDataDir::TestData(QString::fromUtf8("core/tests/database/testimages"))
                           .root().path() + QLatin1Char('/');
    qCDebug(DIGIKAM_TESTS_LOG) << "Test Data Dir:" << filesPath;

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
    QString switchCondition = QLatin1String(argv[1]);

    // ------------------------------------------------------------------------------------

    if (switchCondition == QLatin1String("sqliteToMysql"))
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Setup sqlite Database...";

        DbEngineParameters params;

        params.databaseType = DbEngineParameters::SQLiteDatabaseType();
        params.setCoreDatabasePath(tempDir.path() + QLatin1String("/digikam-core-test.db"));
        params.setThumbsDatabasePath(tempDir.path() + QLatin1String("/digikam-thumbs-test.db"));
        params.setFaceDatabasePath(tempDir.path() + QLatin1String("/digikam-faces-test.db"));
        params.setSimilarityDatabasePath(tempDir.path() + QLatin1String("/digikam-similarity-test.db"));
        params.legacyAndDefaultChecks();

        qCDebug(DIGIKAM_TESTS_LOG) << "Initializing database...";
        bool b = AlbumManager::instance()->setDatabase(params, false, filesPath);

        qCDebug(DIGIKAM_TESTS_LOG) << "Database initialization done: " << b;

        QTest::qWait(3000);
/*
        qCDebug(DIGIKAM_TESTS_LOG) << "Shutting down database";
        ScanController::instance()->shutDown();
        AlbumManager::instance()->cleanUp();
*/
        qCDebug(DIGIKAM_TESTS_LOG) << "Cleaning DB now";
        CoreDbAccess::cleanUpDatabase();
        ThumbsDbAccess::cleanUpDatabase();
        FaceDbAccess::cleanUpDatabase();

        // ------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_TESTS_LOG) << "Setup mysql Database...";

        params                             = DbEngineParameters();
        QString defaultAkDir               = DbEngineParameters::internalServerPrivatePath();
        QString miscDir                    = QDir(defaultAkDir).absoluteFilePath(QLatin1String("db_misc"));
        params.databaseType                = DbEngineParameters::MySQLDatabaseType();
        params.databaseNameCore            = QLatin1String("digikam");
        params.databaseNameThumbnails      = QLatin1String("digikam");
        params.databaseNameFace            = QLatin1String("digikam");
        params.databaseNameSimilarity      = QLatin1String("digikam");
        params.userName                    = QLatin1String("root");
        params.password                    = QString();
        params.internalServer              = true;
        params.internalServerDBPath        = tempDir.path();
        params.internalServerMysqlServCmd  = DbEngineParameters::defaultMysqlServerCmd();
        params.internalServerMysqlInitCmd  = DbEngineParameters::defaultMysqlInitCmd();
        params.internalServerMysqlAdminCmd = DbEngineParameters::defaultMysqlAdminCmd();
        params.hostName                    = QString();
        params.port                        = -1;
        params.connectOptions              = QString::fromLatin1("UNIX_SOCKET=%1/mysql.socket").arg(miscDir);

        // ------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_TESTS_LOG) << "Initializing database...";
        AlbumManager::instance()->changeDatabase(params);

        qCDebug(DIGIKAM_TESTS_LOG) << "Database switch done";

        QTest::qWait(5000);

        qCDebug(DIGIKAM_TESTS_LOG) << "Shutting down database";
        ScanController::instance()->shutDown();
        AlbumManager::instance()->cleanUp();

        qCDebug(DIGIKAM_TESTS_LOG) << "Cleaning DB now";
        CoreDbAccess::cleanUpDatabase();
        ThumbsDbAccess::cleanUpDatabase();
        FaceDbAccess::cleanUpDatabase();

        WSToolUtils::removeTemporaryDir(tempPath.toLatin1().data());
    }

    else if (switchCondition == QLatin1String("mysqlToSqlite"))
    {
        DbEngineParameters params;

        qCDebug(DIGIKAM_TESTS_LOG) << "Setup mysql Database...";

        QString defaultAkDir               = DbEngineParameters::internalServerPrivatePath();
        QString miscDir                    = QDir(defaultAkDir).absoluteFilePath(QLatin1String("db_misc"));
        params.databaseType                = DbEngineParameters::MySQLDatabaseType();
        params.databaseNameCore            = QLatin1String("digikam");
        params.databaseNameThumbnails      = QLatin1String("digikam");
        params.databaseNameFace            = QLatin1String("digikam");
        params.userName                    = QLatin1String("root");
        params.password                    = QString();
        params.internalServer              = true;
        params.internalServerDBPath        = tempDir.path();
        params.internalServerMysqlServCmd  = DbEngineParameters::defaultMysqlServerCmd();
        params.internalServerMysqlInitCmd  = DbEngineParameters::defaultMysqlInitCmd();
        params.internalServerMysqlAdminCmd = DbEngineParameters::defaultMysqlAdminCmd();
        params.hostName                    = QString();
        params.port                        = -1;
        params.connectOptions              = QString::fromLatin1("UNIX_SOCKET=%1/mysql.socket").arg(miscDir);

        // ------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_TESTS_LOG) << "Initializing database...";
        AlbumManager::instance()->changeDatabase(params);

        qCDebug(DIGIKAM_TESTS_LOG) << "Database switch done";

        QTest::qWait(5000);
/*
        qCDebug(DIGIKAM_TESTS_LOG) << "Shutting down database";
        ScanController::instance()->shutDown();
        AlbumManager::instance()->cleanUp();
*/
        qCDebug(DIGIKAM_TESTS_LOG) << "Cleaning DB now";
        CoreDbAccess::cleanUpDatabase();
        ThumbsDbAccess::cleanUpDatabase();
        FaceDbAccess::cleanUpDatabase();

        // ------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_TESTS_LOG) << "Setup sqlite Database...";

        params.databaseType = DbEngineParameters::SQLiteDatabaseType();
        params.setCoreDatabasePath(QDir::currentPath() + QLatin1String("/digikam-core-test.db"));
        params.setThumbsDatabasePath(QDir::currentPath() + QLatin1String("/digikam-thumbs-test.db"));
        params.setFaceDatabasePath(QDir::currentPath() + QLatin1String("/digikam-faces-test.db"));
        params.legacyAndDefaultChecks();

        qCDebug(DIGIKAM_TESTS_LOG) << "Initializing database...";
        bool b = AlbumManager::instance()->setDatabase(params, false, filesPath);

        qCDebug(DIGIKAM_TESTS_LOG) << "Database initialization done: " << b;

        QTest::qWait(3000);

        qCDebug(DIGIKAM_TESTS_LOG) << "Shutting down database";
        ScanController::instance()->shutDown();
        AlbumManager::instance()->cleanUp();

        qCDebug(DIGIKAM_TESTS_LOG) << "Cleaning DB now";
        CoreDbAccess::cleanUpDatabase();
        ThumbsDbAccess::cleanUpDatabase();
        FaceDbAccess::cleanUpDatabase();

        WSToolUtils::removeTemporaryDir(tempPath.toLatin1().data());
    }

    return 0;
}
