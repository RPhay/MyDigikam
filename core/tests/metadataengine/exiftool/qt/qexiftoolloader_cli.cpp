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

#include "exiftoolprocess.h"
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

    QFileInfo fileInfo(QString::fromUtf8(argv[1]));

    if (!fileInfo.exists())
    {
        return false;
    }

    // Post creation of hash tables for tag translations

    ExifToolTranslator::instance();

    // Create ExifTool parser instance.

    ExifToolProcess* const parser  = new ExifToolProcess();

#if defined Q_OS_LINUX || defined Q_OS_MACOS

    parser->setProgram(QLatin1String("/usr/bin/exiftool"));

#elif defined Q_OS_WINDOWS

    parser->setProgram(QLatin1String("exiftool.exe"));

#endif

    // Connect at least cmdCompleted signal to slot

    QObject::connect(parser, &ExifToolProcess::signalCmdCompleted,
                     [=](int /*cmdId*/, int /*execTime*/, const QByteArray& stdOut, const QByteArray& /*stdErr*/)  // clazy:exclude=function-args-by-ref
        {
            // Print returned and sorted tags.

            QString output;
            QTextStream stream(&output);

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
                   << Qt::endl;

            // Convert JSON array as QVariantMap

            QJsonDocument jsonDoc     = QJsonDocument::fromJson(stdOut);
            QJsonArray    jsonArray   = jsonDoc.array();
            QJsonObject   jsonObject  = jsonArray.at(0).toObject();
            QVariantMap   metadataMap = jsonObject.toVariantMap();

            QStringList ignoredETTags;
            QStringList tagsLst;

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
                        ignoredETTags.append(tagNameExifTool.section(QLatin1Char('.'), 0, -2));
                    }

                    continue;
                }

                QString data = it.value().toString();

                if (data.size() > -section3)
                {
                    data = data.left(-section3 - 3) + QLatin1String("...");
                }

                // Tags to translate To Exiv2 naming scheme

                QString tagNameExiv2 = ExifToolTranslator::instance()->translateToExiv2(tagNameExifTool);

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
                stream << tag << Qt::endl;
            }

            stream << sep << Qt::endl;
            ignoredETTags.removeDuplicates();
            stream << "Ignored ExifTool Tags:" << Qt::endl;

            foreach (const QString& itag, ignoredETTags)
            {
                stream << "   " << itag << Qt::endl;
            }

            qDebug().noquote() << output;

            qApp->quit();
        }
    );

    // Read metadata from the file. Start ExifToolProcess

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

    // Send command to ExifToolProcess

    parser->command(cmdArgs); // See additional notes

    app.exec();

    return 0;
}
