/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to test ExifTool image loader
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
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <QVariant>
#include <QObject>

// Local includes

#include "exiftoolparser.h"
#include "exiftooltranslator.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qDebug() << "qexiftoolloader_cli - CLI tool to check ExifTool image loader";
        qDebug() << "Usage: <image>";
        return -1;
    }

    // Create ExifTool parser instance.

    ExifToolParser* const parser = new ExifToolParser();

    // Connect at least cmdCompleted signal to slot

    QObject::connect(parser, &ExifToolParser::signalExifToolMetadata,
                     [=](const ExifToolParser::TagsMap& parsed, const ExifToolParser::TagsMap& ignored)  // clazy:exclude=function-args-by-ref
        {
            // Print returned and sorted tags.

            QString     output;
            QTextStream stream(&output);
            QStringList tagsLst;

            const int section1 = -60;   // ExifTool Tag name
            const int section2 = -45;   // Exiv2 tag name
            const int section3 = -30;   // Tag value as string.
            QString sep        = QString().fill(QLatin1Char('-'), qAbs(section1 + section2 + section3) + 6);

            // Header

            stream << sep
                   << Qt::endl
                   << QString::fromLatin1("%1").arg(QLatin1String("ExifTool::group0.group1.group2.name"), section1) << " | "
                   << QString::fromLatin1("%1").arg(QLatin1String("Exiv2::family.group.name"),            section2) << " | "
                   << QString::fromLatin1("%1").arg(QLatin1String("String Value"),                        section3)
                   << Qt::endl
                   << sep
                   << endl;

            for (ExifToolParser::TagsMap::const_iterator it = parsed.constBegin() ;
                it != parsed.constEnd() ; ++it)
            {
                QString tagNameExifTool = it.value()[0].toString();
                QString tagType         = it.value()[2].toString();
                QString data            = it.value()[1].toString();

                if (data.size() > -section3)
                {
                    data = data.left(-section3 - 3) + QLatin1String("...");
                }

                // Tags to translate To Exiv2 naming scheme

                QString tagNameExiv2    = it.key();

                tagsLst
                        << QString::fromLatin1("%1 | %2 | %3")
                        .arg(tagNameExifTool, section1)
                        .arg(tagNameExiv2,    section2)
                        .arg(data,            section3)
                       ;
            }

            tagsLst.sort();

            foreach (const QString& tag, tagsLst)
            {
                stream << tag << endl;
            }

            stream << sep << endl;
            stream << "Ignored ExifTool Tags:" << endl;

            QStringList itagsLst = ignored.keys();
            itagsLst.sort();

            foreach (const QString& tag, itagsLst)
            {
                stream << "   " << tag << endl;
            }

            qDebug().noquote() << output;

            qApp->quit();
        }
    );

    // Read metadata from the file. Start ExifToolProcess

    if (!parser->parse(QString::fromUtf8(argv[1])))
    {
        return -1;
    }

    app.exec();

    return 0;
}
