/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to get list of files format supported by ExifTool
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
#include <QCoreApplication>
#include <QVariant>

// Local includes

#include "digikam_debug.h"
#include "exiftoolparser.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    // Create ExifTool parser instance.

    ExifToolParser* const parser = new ExifToolParser(qApp);
    QString ext;
    QString desc;
    ExifToolParser::ExifToolData parsed;
    QStringList lst;

    if (parser->readableFormats())
    {
        parsed = parser->currentData();
        lst    = parsed.find(QLatin1String("READ_FORMATS")).value()[0].toStringList();
        qCDebug(DIGIKAM_TESTS_LOG) << "Readable file formats:";

        for (int i = 0 ; i < lst.size() ; i += 2)
        {
            ext  = lst[i];
            desc = lst[i + 1];
            qCDebug(DIGIKAM_TESTS_LOG) << ext << desc;
        }
    }

    if (parser->writableFormats())
    {
        parsed = parser->currentData();
        lst    = parsed.find(QLatin1String("WRITE_FORMATS")).value()[0].toStringList();
        qCDebug(DIGIKAM_TESTS_LOG) << "Writable file formats:";

        for (int i = 0 ; i < lst.size() ; i += 2)
        {
            ext  = lst[i];
            desc = lst[i + 1];
            qCDebug(DIGIKAM_TESTS_LOG) << ext << desc;
        }
    }

    return 0;
}
