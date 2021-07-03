/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to get list of tags from ExifToml database
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
#include <QStringList>
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
    ExifToolParser::ExifToolData parsed;
    QStringList lst;

    if (parser->tagsDatabase())
    {
        parsed           = parser->currentData();
        QStringList keys = parsed.keys();
        keys.sort();
        QString desc;
        QString type;
        QString writable;
        QString output;
        QTextStream stream(&output);
        QStringList tagsLst;

        const int section1 = -65;   // Tag name.
        const int section2 = -10;   // Tag type.
        const int section3 = -15;   // Tag is writable.
        const int section4 = -30;   // Tag description.
        QString sep        = QString().fill(QLatin1Char('-'), qAbs(section1 + section2 + section3 + section4) + 8);

        // Header

        stream << sep
               << endl
               << QString::fromLatin1("%1").arg(QLatin1String("Name"),        section1) << " | "
               << QString::fromLatin1("%1").arg(QLatin1String("Type"),        section2) << " | "
               << QString::fromLatin1("%1").arg(QLatin1String("Writable"),    section3) << " | "
               << QString::fromLatin1("%1").arg(QLatin1String("Description"), section4)
               << endl
               << sep
               << endl;

        foreach (const QString& tag, keys)
        {
            ExifToolParser::ExifToolData::iterator it = parsed.find(tag);

            if (it != parsed.end())
            {
                desc     = it.value()[0].toString();
                type     = it.value()[1].toString();
                writable = it.value()[2].toString();

                tagsLst
                    << QString::fromLatin1("%1 | %2 | %3 | %4")
                        .arg(tag,      section1)
                        .arg(type,     section2)
                        .arg(writable, section3)
                        .arg(desc,     section4)
                ;
            }
        }

        foreach (const QString& tag, tagsLst)
        {
            stream << tag << endl;
        }

        stream << sep << endl;

        qCDebug(DIGIKAM_TESTS_LOG).noquote() << output;

    }

    return 0;
}
