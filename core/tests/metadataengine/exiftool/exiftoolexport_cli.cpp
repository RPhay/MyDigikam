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

#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

// Local includes

#include "dimg.h"
#include "dpluginloader.h"
#include "metaengine.h"
#include "exiftoolcore.h"
#include "exiftooltranslator.h"

using namespace Digikam;

void s_printMetadataMap(const MetaEngine::MetaDataMap& map)
{
    QString output;
    QTextStream stream(&output);
    stream << Qt::endl;

    qDebug() << "Found" << map.size() << "tags:" << Qt::endl;

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

        stream << tagName << " : " << tagVal << Qt::endl;
    }

    qDebug().noquote() << output;
}

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

    QFileInfo fileInfo(QString::fromUtf8(argv[1]));

    if (!fileInfo.exists())
    {
        return false;
    }

    // Create ExifTool parser instance.

    ExifToolCore* const parser  = new ExifToolCore();

    // Post creation of hash tables for tag translations

    ExifToolTranslator::instance();

    // Read metadata from the file.

    ExifToolTagInfo* const info = parser->ImageInfo(fileInfo.filePath().toUtf8().constData(), nullptr, 5);

    if      (info)
    {
        QStringList ignoredETTags;
        QStringList tagsLst;
        MetaEngine  meta;

        for (ExifToolTagInfo* it = info ; it ; it = it->next)
        {
            QString grp0            = QString::fromLatin1(it->group[0]).simplified();                 // Tag group section 0
            QString grp1            = QString::fromLatin1(it->group[1]).simplified();                 // Tag group section 1
            QString grp2            = QString::fromLatin1(it->group[2]).simplified();                 // Tag group section 2
            QString name            = QString::fromLatin1(it->name).simplified();                     // Tag name
            QString tagNameExifTool = QString::fromLatin1("%1.%2.%3.%4").arg(grp0).arg(grp1).arg(grp2).arg(name).simplified();

            if (ExifToolTranslator::instance()->isIgnoredGroup(tagNameExifTool))
            {
                continue;
            }

            QByteArray data         = QByteArray(it->num, it->numLen);

            // Tags to translate To Exiv2 naming scheme

            QString tagNameExiv2 = ExifToolTranslator::instance()->translateToExiv2(tagNameExifTool);

            if      (tagNameExiv2.startsWith(QLatin1String("Exif.")))
            {
                meta.setExifTagData(tagNameExiv2.toLatin1().constData(), data);
            }
            else if (tagNameExiv2.startsWith(QLatin1String("Iptc.")))
            {
                meta.setIptcTagData(tagNameExiv2.toLatin1().constData(), data);
            }
            else if (tagNameExiv2.startsWith(QLatin1String("Xmp.")))
            {
                meta.setXmpTagString(tagNameExiv2.toLatin1().constData(), QString::fromUtf8(data));
            }
        }

        delete info;

        qDebug() << QString::fromUtf8("-- Exif metadata from %1 --").arg(meta.getFilePath());

        MetaEngine::MetaDataMap map = meta.getExifTagsDataList();
        s_printMetadataMap(map);

        qDebug() << QString::fromUtf8("-- Iptc metadata from %1 --").arg(meta.getFilePath());

        map = meta.getIptcTagsDataList();
        s_printMetadataMap(map);

        if (meta.supportXmp())
        {
            qDebug() << QString::fromUtf8("-- Xmp metadata from %1 --").arg(meta.getFilePath());
            map = meta.getXmpTagsDataList();
            s_printMetadataMap(map);
        }
        else
        {
            qWarning() << "Exiv2 has no XMP support...";
        }

        DImg file(1, 1, false);
        file.setMetadata(meta.data());
        file.save(QLatin1String("output.jpg"), DImg::JPEG);
    }
    else if (parser->LastComplete() <= 0)
    {
        qWarning() << "Cannot load" << fileInfo.filePath();
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
