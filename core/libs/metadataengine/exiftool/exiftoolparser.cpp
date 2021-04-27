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

ExifToolParser::ExifToolParser(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
    // Create ExifTool parser instance.

    d->proc      = new ExifToolProcess(this);
    d->loopLoad  = new QEventLoop(this);
    d->loopChunk = new QEventLoop(this);
    d->loopApply = new QEventLoop(this);

    connect(MetaEngineSettings::instance(), SIGNAL(signalSettingsChanged()),
            this, SLOT(slotMetaEngineSettingsChanged()));

    d->hdls << connect(d->proc, &ExifToolProcess::signalCmdCompleted,
                       this, &ExifToolParser::slotCmdCompleted);

    d->hdls << connect(d->proc, &ExifToolProcess::signalErrorOccurred,
                       this, &ExifToolParser::slotErrorOccurred);

    d->hdls << connect(d->proc, &ExifToolProcess::signalFinished,
                       this, &ExifToolParser::slotFinished);

    slotMetaEngineSettingsChanged();
}

ExifToolParser::~ExifToolParser()
{
    d->loopLoad->exit();
    delete d->loopLoad;

    d->loopChunk->exit();
    delete d->loopChunk;

    d->loopApply->exit();
    delete d->loopApply;

    foreach (QMetaObject::Connection hdl, d->hdls)
    {
        disconnect(hdl);
    }

    delete d;
}

QString ExifToolParser::currentPath() const
{
    return d->currentPath;
}

ExifToolParser::ExifToolData ExifToolParser::currentData() const
{
    return d->exifToolData;
}

QString ExifToolParser::currentErrorString() const
{
    return d->proc->errorString();
}

bool ExifToolParser::load(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        return false;
    }

    if (!d->prepareProcess())
    {
        return false;
    }

    // Build command (get metadata as JSON array)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-json");
    cmdArgs << QByteArray("-binary");
    cmdArgs << QByteArray("-G:0:1:2:4:6");
    cmdArgs << QByteArray("-n");
    cmdArgs << QByteArray("-l");
    cmdArgs << d->filePathEncoding(fileInfo);
    d->currentPath = fileInfo.path();

    // Send command to ExifToolProcess

    int ret = d->proc->command(cmdArgs, ExifToolProcess::LOAD_METADATA);

    if (ret == 0)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool load command cannot be sent";

        return false;
    }

    d->loopLoad->exec();

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete load for" << path;

    return true;
}

bool ExifToolParser::loadChunk(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        return false;
    }

    if (!d->prepareProcess())
    {
        return false;
    }

    // Build command (get metadata as EXV container for Exiv2)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-TagsFromFile");
    cmdArgs << d->filePathEncoding(fileInfo);
    cmdArgs << QByteArray("-all:all");
    cmdArgs << QByteArray("-o");
    cmdArgs << QByteArray("-.exv");
    d->currentPath = fileInfo.path();

    // Send command to ExifToolProcess

    int ret = d->proc->command(cmdArgs, ExifToolProcess::LOAD_CHUNKS);

    if (ret == 0)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool load chunk command cannot be sent";

        return false;
    }

    d->loopChunk->exec();

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete load chunk for" << path;

    return true;
}

bool ExifToolParser::applyChanges(const QString& path, const ExifToolData& newTags)
{
    if (newTags.isEmpty())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "List of tags to changes with ExifTool is empty";

        return false;
    }

    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        return false;
    }

    if (!d->prepareProcess())
    {
        return false;
    }

    // Build command (set metadata)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-json");

    for (ExifToolParser::ExifToolData::const_iterator it = newTags.constBegin() ;
         it != newTags.constEnd() ; ++it)
    {
        QString  tagNameExifTool = it.key();
        QString  tagValue        = it.value()[0].toString();
        cmdArgs << QString::fromUtf8("-%1=%2").arg(tagNameExifTool).arg(tagValue).toUtf8();
    }

    cmdArgs << d->filePathEncoding(fileInfo);
    d->currentPath = fileInfo.path();

    // Send command to ExifToolProcess

    int ret = d->proc->command(cmdArgs, ExifToolProcess::APPLY_CHANGES);

    if (ret == 0)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool apply changes command cannot be sent";

        return false;
    }

    d->loopApply->exec();

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete apply changes for" << path;

    return true;
}

void ExifToolParser::slotCmdCompleted(int cmdAction,
                                      int execTime,
                                      const QByteArray& stdOut,
                                      const QByteArray& /*stdErr*/)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete command for action" << d->actionString(cmdAction)
                                    << "with elasped time (ms):" << execTime;
/*
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool output:";
    qCDebug(DIGIKAM_METAENGINE_LOG) << "---";
    qCDebug(DIGIKAM_METAENGINE_LOG).noquote() << stdOut;
    qCDebug(DIGIKAM_METAENGINE_LOG) << "---";
*/

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
                QString data         = propsMap.find(QLatin1String("val")).value().toString();
                QString desc         = propsMap.find(QLatin1String("desc")).value().toString();
/*
                qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool json property:" << tagNameExifTool << data;
*/

                if (data.startsWith(QLatin1String("base64:")))
                {
                    data = i18n("binary data...");
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
            // TODO: check ExifTool feedback.
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

void ExifToolParser::slotMetaEngineSettingsChanged()
{
    d->proc->setProgram(MetaEngineSettings::instance()->settings().exifToolPath);

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool path:" << d->proc->program();
}

} // namespace Digikam
