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

// Local includes

#include "digikam_globals.h"

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
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete command for action"
                                    << d->actionString(cmdAction)
                                    << "with elasped time (ms):"
                                    << execTime;

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

                emit signalExifToolDataAvailable();

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

            QStringList lines = out.split(QLatin1Char('\n'), QT_SKIP_EMPTY_PARTS);
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

            QStringList lines = out.split(QLatin1Char('\n'), QT_SKIP_EMPTY_PARTS);
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

            QStringList lines = out.split(QLatin1Char('\n'), QT_SKIP_EMPTY_PARTS);
            QStringList lst;

            foreach (const QString& ln, lines)
            {
                lst << ln.simplified().section(QLatin1String(" - "), 0, 0);
            }

            d->exifToolData.insert(QLatin1String("TRANSLATIONS_LIST"), QVariantList() << lst);
            break;
        }

        case ExifToolProcess::TAGS_DATABASE:
        {
            QString xml = QString::fromUtf8(stdOut);

            QDomDocument doc;

            if (doc.setContent(xml))
            {
                QDomElement docElem = doc.documentElement();

                if (docElem.tagName() == QLatin1String("taginfo"))
                {
                    for (QDomNode n1 = docElem.firstChild() ; !n1.isNull() ; n1 = n1.nextSibling())
                    {
                        QDomElement e1 = n1.toElement();

                        if (!e1.isNull())
                        {
                            if (e1.tagName() == QLatin1String("table"))                           // Top level group
                            {
                                QString g0       = e1.attribute(QLatin1String("g0"));
                                QString g1       = e1.attribute(QLatin1String("g1"));
                                QString g2       = e1.attribute(QLatin1String("g2"));
                                QString type;
                                QString writable;
                                QString tag;
                                QString mainDesc;
                                QString desc;

                                for (QDomNode n2 = e1.firstChild() ; !n2.isNull() ; n2 = n2.nextSibling())
                                {
                                    QDomElement e2 = n2.toElement();

                                    if (!e2.isNull())
                                    {
                                        if      (e2.tagName() == QLatin1String("desc"))          // Main description of group
                                        {
                                            if (e2.attribute(QLatin1String("lang")) == QLatin1String("en"))
                                            {
                                                mainDesc = e2.text();
                                            }

                                            continue;
                                        }
                                        else if (e2.tagName() == QLatin1String("tag"))           // One tag from group
                                        {
                                            QString name = e2.attribute(QLatin1String("name"));
                                            tag          = QString::fromLatin1("%1.%2.%3.%4").arg(g0).arg(g1).arg(g2).arg(name);
                                            type         = e2.attribute(QLatin1String("type"));
                                            writable     = e2.attribute(QLatin1String("writable"));

                                            for (QDomNode n3 = e2.firstChild() ; !n3.isNull() ; n3 = n3.nextSibling())
                                            {
                                                QDomElement e3 = n3.toElement();

                                                if (!e3.isNull())
                                                {
                                                    if (e3.tagName() == QLatin1String("desc"))  // Description of tag
                                                    {
                                                        if (e3.attribute(QLatin1String("lang")) == QLatin1String("en"))
                                                        {
                                                            desc = e3.text();
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    d->exifToolData.insert(tag,
                                                           QVariantList()
                                                                << QString::fromLatin1("%1 - %2").arg(mainDesc).arg(desc)
                                                                << type
                                                                << writable
                                    );
                                }
                            }
                        }
                    }
                }
            }

            break;
        }

        case ExifToolProcess::VERSION_STRING:
        {
            QString out       = QString::fromUtf8(stdOut);
            QStringList lines = out.split(QLatin1Char('\n'), QT_SKIP_EMPTY_PARTS);

            if (!lines.isEmpty())
            {
                d->exifToolData.insert(QLatin1String("VERSION_STRING"), QVariantList() << lines.first());
            }

            break;
        }

        default:
        {
            break;
        }
    }

    d->manageEventLoop(cmdAction);

    emit signalExifToolDataAvailable();

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool parsed command for action" << d->actionString(cmdAction);
    qCDebug(DIGIKAM_METAENGINE_LOG) << d->exifToolData.count() << "properties decoded";
}

void ExifToolParser::slotErrorOccurred(int cmdAction, QProcess::ProcessError error)
{
    qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool process for action" << d->actionString(cmdAction)
                                      << "exited with error:" << error;

    d->manageEventLoop(cmdAction);

    emit signalExifToolDataAvailable();
}

void ExifToolParser::slotFinished(int cmdAction, int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process for action" << d->actionString(cmdAction)
                                    << "finished with code:" << exitCode
                                    << "and status" << exitStatus;

    d->manageEventLoop(cmdAction);

    emit signalExifToolDataAvailable();
}

void ExifToolParser::setOutputStream(int cmdAction,
                                     const QByteArray& cmdOutputChannel,
                                     const QByteArray& cmdErrorChannel)
{
    slotCmdCompleted(cmdAction, 0, cmdOutputChannel, cmdErrorChannel);
}

} // namespace Digikam
