/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to test ExifTool image loader
 *
 * Copyright (C) 2012-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2019 by Phil Harvey, <philharvey66 at gmail dot com>
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
#include "ExifTool.h"

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

        const int section1 = -30;
        const int section2 = -70;
        const int section3 = -60;

        qDebug().noquote()
                 << QString::fromLatin1("%1").arg(QLatin1String("id"),                        section1) << "|"
                 << QString::fromLatin1("%1").arg(QLatin1String("group1.group2.group3.name"), section2) << "="
                 << QString::fromLatin1("%1").arg(QLatin1String("value"),                     section3)
                 << QLatin1String("description")
                 << Qt::endl;

        for (TagInfo* it = info ; it ; it = it->next)
        {
            QString name    = QString::fromLatin1(it->name).simplified();;
            QString id      = QString::fromLatin1(it->id).simplified();;
            QString desc    = QString::fromLatin1(it->desc).simplified();;
            QString value   = QString::fromLatin1(it->value).simplified();
            QString grp0    = QString::fromLatin1(it->group[0]).simplified();;
            QString grp1    = QString::fromLatin1(it->group[1]).simplified();;
            QString grp2    = QString::fromLatin1(it->group[2]).simplified();;

            QString tagName = QString::fromLatin1("%1.%2.%3.%4")
                              .arg(grp0)
                              .arg(grp1)
                              .arg(grp2)
                              .arg(name)
                              .simplified();

            qDebug().noquote()
                 << QString::fromLatin1("%1 | %2 = %3 %4")
                    .arg(id,      section1)
                    .arg(tagName, section2)
                    .arg(value,   section3)
                    .arg(desc);

/*
            qDebug() << it->name << "=" << it->value;
            qDebug() << "   group[0] =" << (it->group[0] ? it->group[0] : "<null>");  // family 0 group name
            qDebug() << "   group[1] =" << (it->group[1] ? it->group[1] : "<null>");  // family 1 group name
            qDebug() << "   group[2] =" << (it->group[2] ? it->group[2] : "<null>");  // family 2 group name
            qDebug() << "   name     =" << (it->name     ? it->name     : "<null>");  // tag name
            qDebug() << "   desc     =" << (it->desc     ? it->desc     : "<null>");  // tag description
            qDebug() << "   id       =" << (it->id       ? it->id       : "<null>");  // tag ID
            qDebug() << "   value    =" << (it->value    ? it->value    : "<null>");  // converted value
            qDebug() << "   valueLen =" << it->valueLen;                              // length of value in bytes (not including null terminator)
            qDebug() << "   num      =" << (it->num      ? it->num      : "<null>");  // "numerical" value
            qDebug() << "   numLen   =" << it->numLen;                                // length of numerical value
            qDebug() << "   copyNum  =" << it->copyNum;                               // copy number for this tag name
*/
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
