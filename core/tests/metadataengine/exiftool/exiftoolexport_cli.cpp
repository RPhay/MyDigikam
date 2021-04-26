/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to export ExifTool metadata
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

#include "dimg.h"
#include "dpluginloader.h"
#include "metaengine.h"
#include "exiftoolparser.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qDebug() << "exiftoolexport_cli - CLI tool to export ExifTool metadata from image";
        qDebug() << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();
    DPluginLoader::instance()->init();

    // Create ExifTool parser instance.

    ExifToolParser* const parser = new ExifToolParser();

    // Read metadata from the file. Start ExifToolParser

    if (!parser->loadChunk(QString::fromUtf8(argv[1])))
    {
        qWarning() << "Metadata chunks cannot be loaded";
        return -1;
    }


    ExifToolParser::TagsMap chunks       = parser->currentParsedTags();

    qDebug() << "Metadata chunks loaded" ;

    ExifToolParser::TagsMap::iterator it = chunks.find(QString::fromUtf8(argv[1]));

    if (it == chunks.end())
    {
        qWarning() << "Metadata chunks is empty";
        return -1;
    }

    QVariantList varLst = it.value();

    MetaEngine meta;

    QByteArray exif = QByteArray::fromBase64(varLst[0].toString().toLatin1());

    if (!exif.isEmpty())
    {
        qDebug() << "Exif chunks size" << exif.size();
        meta.loadFromData(exif);
    }
/*
    QFile ef(QLatin1String("exif.dat"));
    ef.open(QIODevice::WriteOnly);
    ef.write(exif);
    ef.close();
*/
    QByteArray iptc = QByteArray::fromBase64(varLst[1].toString().toLatin1());

    if (!iptc.isEmpty())
    {
        qDebug() << "Iptc chunks size" << iptc.size();
        meta.setIptc(iptc);
    }

    QByteArray xmp = varLst[2].toString().toLatin1();

    if (!xmp.isEmpty())
    {
        qDebug() << "Xmp chunks size" << xmp.size();
        meta.setXmp(xmp);
    }

    QByteArray comment = varLst[3].toString().toLatin1();

    if (!comment.isEmpty())
    {
        qDebug() << "Comment chunks size" << comment.size();

        meta.setComments(comment);
    }

    DImg file(1, 1, false);
    file.setMetadata(meta.data());
    file.save(QLatin1String("output.jpg"), DImg::JPEG);

    return 0;
}
