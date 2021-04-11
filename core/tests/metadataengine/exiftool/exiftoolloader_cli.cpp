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
#include "exiftooltranslator.h"

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

    ExifToolTagInfo* const info    = parser->ImageInfo(input.filePath().toLatin1().constData(), nullptr, 5);

    if      (info)
    {
        // Print returned tags.

        const int section1 = -55;   // ExifTool Tag name
        const int section2 = -35;   // Exiv2 tag name
        const int section3 = -30;   // Tag value as string.

        ExifToolTranslator translator;

        qDebug().noquote()
                 << QString::fromLatin1("%1").arg(QLatin1String("ExiTool::group0.group1.group2.name"), section1) << ":"
                 << QString::fromLatin1("%1").arg(QLatin1String("Exiv2::family.group.name"),           section2) << "="
                 << QString::fromLatin1("%1").arg(QLatin1String("value"),                              section3)
                 << Qt::endl;

        for (ExifToolTagInfo* it = info ; it ; it = it->next)
        {
            QString id      = QString::fromLatin1(it->id).simplified();                       // Tag ID (what is that?)
            QString desc    = QString::fromLatin1(it->desc).simplified();                     // Description
            QString grp0    = QString::fromLatin1(it->group[0]).simplified();                 // Tag group section 0
            QString grp1    = QString::fromLatin1(it->group[1]).simplified();                 // Tag group section 1
            QString grp2    = QString::fromLatin1(it->group[2]).simplified();                 // Tag group section 2
            QString name    = QString::fromLatin1(it->name).simplified();                     // Tag name
            QString value   = QString::fromLatin1(it->value).simplified().left(-section3);    // Tag value as string converted
            QByteArray data = QByteArray(it->num, it->numLen);

            if (value.size() == -section3)
            {
                value = value.left(-section3 - 3) + QLatin1String("...");
            }

            // Tags to translate To Exiv2 naming scheme

            QString tagNameExifTool = QString::fromLatin1("%1.%2.%3.%4").arg(grp0).arg(grp1).arg(grp2).arg(name).simplified();
            QString tagNameExiv2    = translator.translateToExiv2(tagNameExifTool);

            qDebug().noquote()
                 << QString::fromLatin1("%1 : %2 = %3")
                    .arg(tagNameExifTool, section1)
                    .arg(tagNameExiv2,    section2)
                    .arg(value,           section3)
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
