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
#include "exiftooltranslator.h"

using namespace Digikam;

void s_printMetadataMap(const MetaEngine::MetaDataMap& map)
{
    QString output;
    QTextStream stream(&output);
    stream << endl;

    qDebug() << "Found" << map.size() << "tags:" << endl;

    for (MetaEngine::MetaDataMap::const_iterator it = map.constBegin() ;
         it != map.constEnd() ; ++it)
    {
        QString key     = it.key();
        QString value   = it.value();

        QString tagName = key.simplified();
        tagName.append(QString().fill(QLatin1Char(' '), 48 - tagName.length()));

        QString tagVal  = value.simplified();

        if (tagVal.length() > 48)
        {
            tagVal.truncate(48);
            tagVal.append(QString::fromLatin1("... (%1 bytes)").arg(value.length()));
        }

        stream << tagName << " : " << tagVal << endl;
    }

    qDebug().noquote() << output;
}

// -----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qDebug() << "qexiftoolexport_cli - CLI tool to export ExifTool metadata from image";
        qDebug() << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();
    DPluginLoader::instance()->init();

    // Create ExifTool parser instance.

    ExifToolParser* const parser = new ExifToolParser();

    // Read metadata from the file. Start ExifToolParser

    if (!parser->parse(QString::fromUtf8(argv[1])))
    {
        return -1;
    }

    QString path                    = parser->currentParsedPath();
    ExifToolParser::TagsMap parsed  = parser->currentParsedTags();
    ExifToolParser::TagsMap ignored = parser->currentIgnoredTags();

    qDebug().noquote() << "Source File:" << path;

    MetaEngine meta;

    for (ExifToolParser::TagsMap::const_iterator it = parsed.constBegin() ;
         it != parsed.constEnd() ; ++it)
    {
        QString  tagNameExifTool = it.value()[0].toString();
        QString  tagType         = it.value()[2].toString();
        QVariant var             = it.value()[1];
        QString  tagNameExiv2    = it.key();

        if      (tagNameExiv2.startsWith(QLatin1String("Exif.")))
        {
            if (tagType == QLatin1String("undef"))
            {
                // Print ExiTool tags with undefined value type to hack.

                qDebug().noquote() << QString::fromLatin1("%1 %2 %3")
                                        .arg(tagNameExifTool, -60)
                                        .arg(tagType)
                                        .arg(tagNameExiv2, -45)
                                   << var;
            }

            meta.setExifTagVariant(tagNameExiv2.toLatin1().constData(), var);
        }
        else if (tagNameExiv2.startsWith(QLatin1String("Iptc.")))
        {
            meta.setIptcTagString(tagNameExiv2.toLatin1().constData(), var.toString());
        }
        else if (tagNameExiv2.startsWith(QLatin1String("Xmp.")))
        {
            meta.setXmpTagString(tagNameExiv2.toLatin1().constData(), var.toString());
        }
    }

    qDebug().noquote() << QString::fromUtf8("-- Exif metadata from %1 --").arg(meta.getFilePath());

    MetaEngine::MetaDataMap map = meta.getExifTagsDataList();
    s_printMetadataMap(map);

    qDebug().noquote() << QString::fromUtf8("-- Iptc metadata from %1 --").arg(meta.getFilePath());

    map = meta.getIptcTagsDataList();
    s_printMetadataMap(map);

    if (meta.supportXmp())
    {
        qDebug().noquote() << QString::fromUtf8("-- Xmp metadata from %1 --").arg(meta.getFilePath());
        map = meta.getXmpTagsDataList();
        s_printMetadataMap(map);
    }
    else
    {
        qWarning() << "Exiv2 has no XMP support...";
    }

    qDebug().noquote() << "-- Ignored ExifTool Tags --";

    QStringList itagsLst = ignored.keys();
    itagsLst.sort();

    foreach (const QString& tag, itagsLst)
    {
        qDebug().noquote() << "   " << tag;
    }

    // Write all metadata to an empty JPG file.

    DImg file(1, 1, false);
    file.setMetadata(meta.data());
    file.save(QLatin1String("output.jpg"), DImg::JPEG);

    return 0;
}
