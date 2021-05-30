/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftoolparser_p.h"

namespace Digikam
{

void ExifToolParser::printExifToolOutput(const QByteArray& stdOut)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool output:";
    qCDebug(DIGIKAM_METAENGINE_LOG) << "---";
    qCDebug(DIGIKAM_METAENGINE_LOG).noquote() << stdOut;
    qCDebug(DIGIKAM_METAENGINE_LOG) << "---";
}

void ExifToolParser::slotCmdCompleted(int cmdAction,
                                      int execTime,
                                      const QByteArray& stdOut,
                                      const QByteArray& /*stdErr*/)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete command for action" << d->actionString(cmdAction)
                                    << "with elasped time (ms):" << execTime;

    switch (cmdAction)
    {
        case ExifToolProcess::LOAD_METADATA:
        {
            // Convert JSON array as QVariantMap

            QJsonDocument jsonDoc     = QJsonDocument::fromJson(stdOut);
            QJsonArray    jsonArray   = jsonDoc.array();

            qCDebug(DIGIKAM_METAENGINE_LOG) << "Json Array size:" << jsonArray.size();

            if (jsonArray.size() == 0)
            {
                d->manageEventLoop(cmdAction);

                return;
            }

            QJsonObject   jsonObject  = jsonArray.at(0).toObject();
            QVariantMap   metadataMap = jsonObject.toVariantMap();

            qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool Json map size:" << metadataMap.size();

            for (QVariantMap::const_iterator it = metadataMap.constBegin() ;
                it != metadataMap.constEnd() ; ++it)
            {
                QString     tagNameExifTool;
                QString     tagType;
                QStringList sections  = it.key().split(QLatin1Char(':'));

                if      (sections.size() == 6)      // With ExifTool > 12.00 (at least under Windows or MacOS), groups are return with 6 sections.
                {
                    tagNameExifTool = QString::fromLatin1("%1.%2.%3.%4")
                                          .arg(sections[0])
                                          .arg(sections[1])
                                          .arg(sections[2])
                                          .arg(sections[5]);
                    tagType         = sections[4];
                }
                else if (sections.size() == 5)      // ExifTool 12.00 under Linux return 5 or 4 sections.
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
                else if (sections[0] == QLatin1String("SourceFile"))
                {
                    d->currentPath = it.value().toString();
                    continue;
                }
                else
                {
                    continue;
                }

                QVariantMap propsMap = it.value().toMap();
                QString data;

                if (propsMap.find(QLatin1String("val")).value().type() == QVariant::List)
                {
                    QStringList list = propsMap.find(QLatin1String("val")).value().toStringList();
                    data             = list.join(QLatin1String(", "));
                }
                else
                {
                    data             = propsMap.find(QLatin1String("val")).value().toString();
                }

                QString desc         = propsMap.find(QLatin1String("desc")).value().toString();
/*
                qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool json property:" << tagNameExifTool << data;
*/

                if (data.startsWith(QLatin1String("(Binary data ")) && 
                    data.endsWith(QLatin1String(", use -b option to extract)")))
                {
                    data = data.section(QLatin1Char(','), 0, 0);
                    data.remove(QLatin1Char('('));
                }

                d->exifToolData.insert(tagNameExifTool, QVariantList()
                                                         << data        // ExifTool Raw data as string.
                                                         << tagType     // ExifTool data type.
                                                         << desc);      // ExifTool tag description.
            }

            break;
        }

        case ExifToolProcess::LOAD_CHUNKS:
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "EXV" << stdOut.size();

            d->exifToolData.insert(QLatin1String("EXV"), QVariantList() << stdOut);     // Exv chunk as bytearray.
            break;
        }

        case ExifToolProcess::APPLY_CHANGES:
        {
            printExifToolOutput(stdOut);
            break;
        }

        case ExifToolProcess::APPLY_CHANGES_EXV:
        {
            printExifToolOutput(stdOut);
            break;
        }

        case ExifToolProcess::COPY_TAGS:
        {
            printExifToolOutput(stdOut);
            break;
        }

        case ExifToolProcess::TRANS_TAGS:
        {
            printExifToolOutput(stdOut);

            if (!d->argsFile.isOpen() && d->argsFile.exists())
            {
                d->argsFile.remove();
            }

            break;
        }

        case ExifToolProcess::READ_FORMATS:
        {
            // Remove first line

            QString out       = QString::fromUtf8(stdOut).section(QLatin1Char('\n'), 1, -1);

            // Get extensions and descriptions as pair of strings

            QStringList lines = out.split(QLatin1Char('\n'), QString::SkipEmptyParts);
            QStringList lst;
            QString s;

            foreach (const QString& ln, lines)
            {
                s            = ln.simplified();
                QString ext  = s.section(QLatin1Char(' '), 0, 0);
                QString desc = s.section(QLatin1Char(' '), 1, -1);
                lst << ext << desc;
            }


            d->exifToolData.insert(QLatin1String("READ_FORMATS"), QVariantList() << lst);
            break;
        }

        case ExifToolProcess::WRITE_FORMATS:
        {
            // Remove first line

            QString out       = QString::fromUtf8(stdOut).section(QLatin1Char('\n'), 1, -1);

            // Get extensions and descriptions as pair of strings

            QStringList lines = out.split(QLatin1Char('\n'), QString::SkipEmptyParts);
            QStringList lst;
            QString s;

            foreach (const QString& ln, lines)
            {
                s            = ln.simplified();
                QString ext  = s.section(QLatin1Char(' '), 0, 0);
                QString desc = s.section(QLatin1Char(' '), 1, -1);
                lst << ext << desc;
            }

            d->exifToolData.insert(QLatin1String("WRITE_FORMATS"), QVariantList() << lst);
            break;
        }

        case ExifToolProcess::TRANSLATIONS_LIST:
        {
            // Remove first line

            QString out       = QString::fromUtf8(stdOut).section(QLatin1Char('\n'), 1, -1);

            // Get i18n list

            QStringList lines = out.split(QLatin1Char('\n'), QString::SkipEmptyParts);
            QStringList lst;

            foreach (const QString& ln, lines)
            {
                lst << ln.simplified().section(QLatin1String(" - "), 0, 0);
            }

            d->exifToolData.insert(QLatin1String("TRANSLATIONS_LIST"), QVariantList() << lst);
            break;
        }

        default:
        {
            break;
        }
    }

    d->manageEventLoop(cmdAction);

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool parsed command for action" << d->actionString(cmdAction);
    qCDebug(DIGIKAM_METAENGINE_LOG) << d->exifToolData.count() << "properties decoded";
}

void ExifToolParser::slotErrorOccurred(int cmdAction, QProcess::ProcessError error)
{
    qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool process for action" << d->actionString(cmdAction)
                                      << "exited with error:" << error;

    d->manageEventLoop(cmdAction);
}

void ExifToolParser::slotFinished(int cmdAction, int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process for action" << d->actionString(cmdAction)
                                    << "finished with code:" << exitCode
                                    << "and status" << exitStatus;

    d->manageEventLoop(cmdAction);
}

void ExifToolParser::setOutputStream(int cmdAction,
                                     const QByteArray& cmdOutputChannel,
                                     const QByteArray& cmdErrorChannel)
{
    slotCmdCompleted(cmdAction, 0, cmdOutputChannel, cmdErrorChannel);
}

} // namespace Digikam
