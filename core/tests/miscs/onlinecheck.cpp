/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : an unit test to check version online.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QTest>
#include <QDateTime>
#include <QCommandLineParser>
#include <QDebug>

// KDE includes

#include <kaboutdata.h>

// Local includes

#include "digikam_config.h"
#include "digikam_version.h"
#include "daboutdata.h"
#include "onlineversiondlg.h"

using namespace Digikam;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (argc < 3)
    {
        qDebug() << "onlinecheck <bool> <bool> - Check if new version is online";
        qDebug() << "Usage: <bool> 0 for stable release only, 1 for pre-release.";
        qDebug() << "       <bool> 0 without debug symbols, 1 with debug symbols.";
        return -1;
    }

    bool preRelease = QString::fromLatin1(argv[1]).toInt();
    bool withDebug  = QString::fromLatin1(argv[2]).toInt();

    qDebug() << "Check for pre-release     :" << preRelease;
    qDebug() << "Version with debug symbols:" << withDebug;

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

    OnlineVersionDlg* const dlg = new OnlineVersionDlg(nullptr,
                                                       QLatin1String("7.0.0"),
                                                       QDateTime::fromString(QLatin1String("2021-01-01T00:00:00"), Qt::ISODate),
                                                       preRelease,
                                                       withDebug);

    return (dlg->exec());
}
