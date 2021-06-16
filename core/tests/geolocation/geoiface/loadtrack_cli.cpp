/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-07-02
 * Description : Simple program to load a track for timing tests.
 *
 * Copyright (C) 2014 by Michael G. Hansen <mike at mghansen dot de>
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

#include <QDateTime>
#include <QTextStream>
#include <QCoreApplication>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "trackmanager.h"
#include "trackreader.h"

using namespace Digikam;

namespace
{
    QTextStream qout(stdout);
    QTextStream qerr(stderr);
}

/**
 * @brief Test loading of a GPX file directly
 */
bool testSaxLoader(const QString& filename)
{
    TrackReader::TrackReadResult fileData = TrackReader::loadTrackFile(QUrl::fromLocalFile(filename));

    return fileData.isValid;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    if (argc < 2)
    {
        qerr << QLatin1String("Need a filename as argument to load") << QT_ENDL;
        return 1;
    }

    const QString filename = QString::fromLatin1(argv[1]);
    qerr << "Loading file: " << filename << QT_ENDL;
    const bool success     = testSaxLoader(filename);

    if (!success)
    {
        qerr << "Loading failed" << QT_ENDL;
        return 1;
    }

    qerr << "Loaded successfully." << QT_ENDL;

    return 0;
}
