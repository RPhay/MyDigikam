/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to test ExifTool image loader
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2019 by Phil Harvey <philharvey66 at gmail dot com>
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

#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

// Local includes

#include "dmetadata.h"
#include "exiftool.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qDebug() << "exiftoolloader_cli - CLI tool to check ExifTool image loader";
        qDebug() << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();

    QFileInfo input(QString::fromUtf8(argv[1]));

    // Create ExifTool parser instance.

    ExifTool* const parser = new ExifTool();

    // Read metadata from the file.

    TagInfo* const info    = parser->ImageInfo(input.filePath().toLatin1().constData(), nullptr, 5);

    if      (info)
    {
        // Print returned tags.

//        const int section1 = -30;
        const int section2 = -70;
        const int section3 = -60;

        qDebug().noquote()
//                 << QString::fromLatin1("%1").arg(QLatin1String("id"),                        section1) << "|"
                 << QLatin1String(" ")
                 << QString::fromLatin1("%1").arg(QLatin1String("group0.group1.group2.name"), section2) << "="
                 << QString::fromLatin1("%1").arg(QLatin1String("value"),                     section3)
//                 << QLatin1String("description")
                 << Qt::endl;

        for (TagInfo* it = info ; it ; it = it->next)
        {
            QString name    = QString::fromLatin1(it->name).simplified();
            QString id      = QString::fromLatin1(it->id).simplified();
            QString desc    = QString::fromLatin1(it->desc).simplified();
            QString value   = QString::fromLatin1(it->value).simplified();
            QString grp0    = QString::fromLatin1(it->group[0]).simplified();
            QString grp1    = QString::fromLatin1(it->group[1]).simplified();
            QString grp2    = QString::fromLatin1(it->group[2]).simplified();

            // Tags to ignore

            if (
                (grp0.isEmpty()                      && grp1.isEmpty()                      && grp2.isEmpty())                      ||
                ((grp0 == QLatin1String("ExifTool")) && (grp1 == QLatin1String("ExifTool")) && (grp2 == QLatin1String("ExifTool"))) ||
                ((grp0 == QLatin1String("File"))     && (grp1 == QLatin1String("File"))     && (grp2 == QLatin1String("Other")))    ||
                ((grp0 == QLatin1String("File"))     && (grp1 == QLatin1String("System"))   && (grp2 == QLatin1String("Other")))    ||
                ((grp0 == QLatin1String("File"))     && (grp1 == QLatin1String("System"))   && (grp2 == QLatin1String("Time")))
               )
            {
                continue;
            }

            // Tags to translate To Exiv2 naming scheme

            QString tagName;
            QString translated = QLatin1String("*");

            if      ((grp0 == QLatin1String("EXIF")) && (grp1 == QLatin1String("IFD0")) && (grp2 == QLatin1String("Image")))
            {
                tagName = QString::fromLatin1("Exif.Image.%1").arg(name);
            }
            else if ((grp0 == QLatin1String("EXIF")) && (grp1 == QLatin1String("IFD1")) && (grp2 == QLatin1String("Image")))
            {
                tagName = QString::fromLatin1("Exif.Image.%1").arg(name);
            }
            else
            {
                // Original from ExifTool

                translated = QLatin1String(" ");
                tagName    = QString::fromLatin1("%1.%2.%3.%4")
                             .arg(grp0)
                             .arg(grp1)
                             .arg(grp2)
                             .arg(name)
                             .simplified();
            }

            qDebug().noquote()
//                 << QString::fromLatin1("%1 |").arg(id, section1)
                 << QString::fromLatin1("%1 %2 = %3")
                    .arg(translated)
                    .arg(tagName, section2)
                    .arg(value,   section3)
//                 << QString::fromLatin1("%1").arg(desc)
                 ;

        }

        delete info;
    }
    else if (parser->LastComplete() <= 0)
    {
        qWarning() << "Cannot load" << input.filePath();
    }

    // Print ExifTool errors.

    QString err = QString::fromLatin1(parser->GetError());

    if (!err.isEmpty())
    {
        qWarning() << err;
    }

    delete parser;

    return 0;
}
