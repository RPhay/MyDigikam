/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to test ExifTool stream parsing.
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QString>
#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QObject>

// Local includes

#include "digikam_debug.h"
#include "exiftoolparser.h"
#include "exiftoolprocess.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "exiftoolparserout_cli - CLI tool to check ExifTool stream parsing";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <ExifTool input stream>";
        return -1;
    }

    QFile input(QString::fromUtf8(argv[1]));

    if (!input.open(QIODevice::ReadOnly))
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Cannot open ExifTool input stream to read...";
        return false;
    }

    QByteArray stdout = input.readAll();

    ExifToolParser* const parser = new ExifToolParser(qApp);

    parser->setOutputStream(ExifToolProcess::LOAD_METADATA,     // Command action ID
                            stdout,                             // Output channel
                            QByteArray());                      // Error channel

    return 0;
}
