/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : ExifTool JSON parser
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftoolparser.h"

// Qt includes

#include <QFileInfo>
#include <QStringList>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// Local includes

#include "qexiftoolprocess.h"
#include "exiftooltranslator.h"
#include "metaengine.h"
#include "digikam_debug.h"

namespace Digikam
{

ExifToolParser::ExifToolParser(QObject* const parent)
    : QObject(parent)
{
    // Post creation of hash tables for tag translations

    ExifToolTranslator::instance();

    // Create ExifTool parser instance.

    m_proc = new QExifToolProcess(parent);

#if defined Q_OS_LINUX || defined Q_OS_MACOS

    m_proc->setProgram(QLatin1String("/usr/bin/exiftool"));

#elif defined Q_OS_WINDOWS

    m_proc->setProgram(QLatin1String("exiftool.exe"));

#endif

    // Connect at least cmdCompleted signal to slot

    connect(m_proc, &QExifToolProcess::signalCmdCompleted,
            this, &ExifToolParser::slotCmdCompleted);
}

ExifToolParser::~ExifToolParser()
{
    delete m_proc;
}

bool ExifToolParser::parse(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        return false;
    }

    // Read metadata from the file. Start QExifToolProcess

    m_proc->start();

    if (!m_proc->waitForStarted(500))
    {
        m_proc->kill();
        return false;
    }

    // Build command (get metadata as JSON array)

    QByteArrayList cmdArgs;
    cmdArgs << "-json";
    cmdArgs << "-binary";
    cmdArgs << "-G:0:1:2:4:6";
    cmdArgs << "-n";
    cmdArgs << fileInfo.filePath().toUtf8();

    // Send command to QExifToolProcess

    int ret = m_proc->command(cmdArgs); // See additional notes

    return (ret != 0);
}

void ExifToolParser::slotCmdCompleted(int /*cmdId*/,
                                      int /*execTime*/,
                                      const QByteArray& stdOut,
                                      const QByteArray& /*stdErr*/)
{
    // Convert JSON array as QVariantMap

    QJsonDocument jsonDoc     = QJsonDocument::fromJson(stdOut);
    QJsonArray    jsonArray   = jsonDoc.array();
    QJsonObject   jsonObject  = jsonArray.at(0).toObject();
    QVariantMap   metadataMap = jsonObject.toVariantMap();

    QStringList tagsLst;
    TagsMap outMap;
    TagsMap ignoredMap;

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

        QString data = it.value().toString();

        if (ExifToolTranslator::instance()->isIgnoredGroup(tagNameExifTool))
        {
            if (!tagNameExifTool.startsWith(QLatin1String("...")))
            {
                ignoredMap.insert(tagNameExifTool, QVariantList() << QString() << data << tagType);
            }

            continue;
        }

        // Tags to translate To Exiv2 naming scheme

        QString tagNameExiv2 = ExifToolTranslator::instance()->translateToExiv2(tagNameExifTool);
        QVariant var;

        if      (tagNameExiv2.startsWith(QLatin1String("Exif.")))
        {
            if      (tagType == QLatin1String("string"))
            {
                var = data;
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
            }
            else if (
                     (tagType == QLatin1String("double"))      ||
                     (tagType == QLatin1String("float"))       ||
                     (tagType == QLatin1String("rational64s")) ||
                     (tagType == QLatin1String("rational64u"))
                    )
            {
                var = data.toDouble();
            }
            else
            {
                ignoredMap.insert(tagNameExiv2, QVariantList() << tagNameExifTool << data << tagType);
            }
        }
        else if (tagNameExiv2.startsWith(QLatin1String("Iptc.")))
        {
            var = data;
        }
        else if (tagNameExiv2.startsWith(QLatin1String("Xmp.")))
        {
            var = data;
        }

        outMap.insert(tagNameExiv2, QVariantList() << tagNameExifTool << var << tagType);
    }

    emit signalExifToolMetadata(outMap, ignoredMap);
}

} // namespace Digikam
