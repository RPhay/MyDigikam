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

    // create our ExifTool object

    ExifTool* const et = new ExifTool();

    // read metadata from the image

    TagInfo* const info = et->ImageInfo(input.filePath().toLatin1().constData(), nullptr, 5);

    if (info)
    {
        // print returned information

        for (TagInfo* it = info ; it ; it = it->next)
        {
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
        }

        // we are responsible for deleting the information when done

        delete info;
    }
    else if (et->LastComplete() <= 0)
    {
        qWarning() << "Cannot load" << input.filePath();
    }

    // print exiftool stderr messages

    char* const err = et->GetError();

    if (err)
    {
        qWarning() << err;
    }

    delete et;      // delete our ExifTool object

    return 0;
}
