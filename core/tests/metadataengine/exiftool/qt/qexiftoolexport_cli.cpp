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
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

// Local includes

#include "dimg.h"
#include "dpluginloader.h"
#include "metaengine.h"
#include "qexiftoolprocess.h"
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
        qDebug() << "qexiftoolexport_cli - CLI tool to export ExifTool metadata from image";
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

    // Post creation of hash tables for tag translations

    ExifToolTranslator::instance();

    // Create ExifTool parser instance.

    QExifToolProcess* const parser  = new QExifToolProcess();

#if defined Q_OS_LINUX || defined Q_OS_MACOS

    parser->setProgram(QLatin1String("/usr/bin/exiftool"));

#elif defined Q_OS_WINDOWS

    parser->setProgram(QLatin1String("exiftool.exe"));

#endif

    // Connect at least cmdCompleted signal to slot

    QObject::connect(parser, &QExifToolProcess::signalCmdCompleted,
                     [=](int /*cmdId*/, int /*execTime*/, const QByteArray& stdOut, const QByteArray& /*stdErr*/)  // clazy:exclude=function-args-by-ref
        {
            // Convert JSON array as QVariantMap

            QJsonDocument jsonDoc     = QJsonDocument::fromJson(stdOut);
            QJsonArray    jsonArray   = jsonDoc.array();
            QJsonObject   jsonObject  = jsonArray.at(0).toObject();
            QVariantMap   metadataMap = jsonObject.toVariantMap();

            QStringList ignoredETTags;
            QStringList tagsLst;
            MetaEngine  meta;

            for (QVariantMap::const_iterator it = metadataMap.constBegin() ;
                it != metadataMap.constEnd() ; ++it)
            {
                QString tagNameExifTool;
                QString tagType;
                QStringList sections    = it.key().split(QLatin1Char(':'));

                if      (sections.size() == 5)
                {
                    tagNameExifTool = QString::fromLatin1("%1.%2.%3.%4")
                                          .arg(sections[0])
                                          .arg(sections[1])
                                          .arg(sections[2])
                                          .arg(sections[4]);
                    tagType         = sections[3];
                }
                else if (sections.size() == 4)
                {
                    tagNameExifTool = QString::fromLatin1("%1.%2.%3.%4")
                                          .arg(sections[0])
                                          .arg(sections[1])
                                          .arg(sections[2])
                                          .arg(sections[3]);
                }
                else
                {
                    continue;
                }

                if (ExifToolTranslator::instance()->isIgnoredGroup(tagNameExifTool))
                {
                    if (!tagNameExifTool.startsWith(QLatin1String("...")))
                    {
                        ignoredETTags.append(tagNameExifTool);
                    }

                    continue;
                }

                QString data = it.value().toString();

                // Tags to translate To Exiv2 naming scheme

                QString tagNameExiv2 = ExifToolTranslator::instance()->translateToExiv2(tagNameExifTool);

                if      (tagNameExiv2.startsWith(QLatin1String("Exif.")))
                {
                    QVariant var;

                    if      (tagType == QLatin1String("string"))
                    {
                        var = data;
                        meta.setExifTagVariant(tagNameExiv2.toLatin1().constData(), var);
                    }
                    else if (
                             (tagType == QLatin1String("int8u"))  ||
                             (tagType == QLatin1String("int16u")) ||
                             (tagType == QLatin1String("int32u")) ||
                             (tagType == QLatin1String("int8s"))  ||
                             (tagType == QLatin1String("int16s")) ||
                             (tagType == QLatin1String("int32s"))
                            )
                    {
                        var = data.toLongLong();
                        meta.setExifTagVariant(tagNameExiv2.toLatin1().constData(), var);
                    }
                    else if (tagType == QLatin1String("undef"))
                    {
                        if (
                            (tagNameExiv2 == QLatin1String("Exif.Photo.ComponentsConfiguration")) ||
                            (tagNameExiv2 == QLatin1String("Exif.Photo.SceneType"))               ||
                            (tagNameExiv2 == QLatin1String("Exif.Photo.FileSource"))
                            )
                        {
                            QByteArray conv;
                            QStringList vals = data.split(QLatin1Char(' '));

                            foreach (const QString& v, vals)
                            {
                                conv.append(QString::fromLatin1("0x%1").arg(v.toInt(), 2, 16).toLatin1());
                            }

                            var = QByteArray::fromHex(conv);
                        }
                        else
                        {
                            var = data.toLatin1();
                        }

                        qDebug().noquote()
                                 << QString::fromLatin1("%1").arg(tagNameExiv2, -45)
                                 << QString::fromLatin1("%1").arg(tagType,      -16)
                                 << QString::fromLatin1("%1").arg(data,         -20)
                                 << var;

                        meta.setExifTagVariant(tagNameExiv2.toLatin1().constData(), var);
                    }
                    else if (
                             (tagType == QLatin1String("double"))      ||
                             (tagType == QLatin1String("float"))       ||
                             (tagType == QLatin1String("rational64s")) ||
                             (tagType == QLatin1String("rational64u"))
                            )
                    {
                        var = data.toDouble();
                        meta.setExifTagVariant(tagNameExiv2.toLatin1().constData(), var);
                    }
                    else
                    {
                        ignoredETTags.append(
                                 QString::fromLatin1("%1, %2, %3, %4")
                                    .arg(tagNameExifTool)
                                    .arg(tagNameExiv2)
                                    .arg(tagType)
                                    .arg(data));
                    }
                }
                else if (tagNameExiv2.startsWith(QLatin1String("Iptc.")))
                {
                    meta.setIptcTagString(tagNameExiv2.toLatin1().constData(), data);
                }
                else if (tagNameExiv2.startsWith(QLatin1String("Xmp.")))
                {
                    meta.setXmpTagString(tagNameExiv2.toLatin1().constData(), data);
                }
            }

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

            qDebug() << "-- Ignored ExifTool Tags --";

            foreach (const QString& itag, ignoredETTags)
            {
                qDebug() << "   " << itag;
            }

            DImg file(1, 1, false);
            file.setMetadata(meta.data());
            file.save(QLatin1String("output.jpg"), DImg::JPEG);

            qApp->quit();
        }
    );

    // Read metadata from the file. Start QExifToolProcess

    parser->start();

    if (!parser->waitForStarted(500))
    {
        parser->kill();
        return -1;
    }

    // Build command (get metadata as JSON array)

    QByteArrayList cmdArgs;
    cmdArgs << "-json";
    cmdArgs << "-binary";
    cmdArgs << "-G:0:1:2:4:6";
    cmdArgs << "-n";
    cmdArgs << fileInfo.filePath().toUtf8();

    // Send command to QExifToolProcess

    parser->command(cmdArgs); // See additional notes

    app.exec();

    return 0;
}
