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

bool ExifToolParser::load(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open source file to process with ExifTool...";
        return false;
    }

    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    // Build command (get metadata as JSON array)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-json");
    cmdArgs << QByteArray("-G:0:1:2:4:6");
    cmdArgs << QByteArray("-n");
    cmdArgs << QByteArray("-l");

/*
    TODO: better i18n support from ExifTool.
    ExifTool Translations are limited to few languages, and passing a non supported code to ExifTool
    returns an error. So we needs a mechanism to get the complete list of cuurent i18n code supported, 
    typically "exiftool -lang".

    // Forward GUI language code to get translated string from ExifTool (typically tags descriptions)

    QStringList langs = QLocale().uiLanguages();
    QByteArray lg;

    if (!langs.isEmpty())
    {
        lg = langs.first().section(QLatin1Char('-'), 0, 0).toLatin1();
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "UI language code:" << lg;

    if (!lg.isEmpty())
    {
        cmdArgs << QByteArray("-lang") << lg;
    }

*/

    cmdArgs << d->filePathEncoding(fileInfo);
    d->currentPath = fileInfo.filePath();

    return (d->startProcess(cmdArgs, ExifToolProcess::LOAD_METADATA));
}

bool ExifToolParser::loadChunk(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open source file to process with ExifTool...";
        return false;
    }

    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    // Build command (get metadata as EXV container for Exiv2)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-TagsFromFile");
    cmdArgs << d->filePathEncoding(fileInfo);
    cmdArgs << QByteArray("-all:all");
    cmdArgs << QByteArray("'-xmp:all<all'");
    cmdArgs << QByteArray("-o");
    cmdArgs << QByteArray("-.exv");
    d->currentPath = fileInfo.filePath();

    return (d->startProcess(cmdArgs, ExifToolProcess::LOAD_CHUNKS));
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
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open source file to process with ExifTool...";
        return false;
    }

    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
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
    d->currentPath = fileInfo.filePath();

    return (d->startProcess(cmdArgs, ExifToolProcess::APPLY_CHANGES));
}

bool ExifToolParser::applyChanges(const QString& path, const QString& exvTempFile)
{
    if (exvTempFile.isEmpty())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "EXV container files to apply changes with ExifTool is empty";
        return false;
    }

    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open source file to process with ExifTool...";
        return false;
    }

    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    QByteArrayList cmdArgs;

    cmdArgs << QByteArray("-TagsFromFile");
    cmdArgs << d->filePathEncoding(exvTempFile);
    cmdArgs << QByteArray("-all:all");
    cmdArgs << d->filePathEncoding(fileInfo);
    d->currentPath = fileInfo.filePath();

    return (d->startProcess(cmdArgs, ExifToolProcess::APPLY_CHANGES_EXV));
}

bool ExifToolParser::readableFormats()
{
    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    // Build command (set metadata)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-l");
    cmdArgs << QByteArray("-listr");

    d->currentPath.clear();

    return (d->startProcess(cmdArgs, ExifToolProcess::READ_FORMATS));
}

bool ExifToolParser::writableFormats()
{
    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    // Build command (set metadata)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-l");
    cmdArgs << QByteArray("-listwf");

    d->currentPath.clear();

    return (d->startProcess(cmdArgs, ExifToolProcess::WRITE_FORMATS));
}

bool ExifToolParser::translationsList()
{
    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    // Build command (set metadata)

    QByteArrayList cmdArgs;
    cmdArgs << QByteArray("-lang");

    d->currentPath.clear();

    return (d->startProcess(cmdArgs, ExifToolProcess::TRANSLATIONS_LIST));
}

bool ExifToolParser::copyTags(const QString& src, const QString& dst,
                              unsigned char copyOps,
                              unsigned char writeModes)
{
    QFileInfo sfi(src);

    if (!sfi.exists())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open source file to process with ExifTool...";
        return false;
    }

    QFileInfo dfi(src);

    if (!dfi.exists())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open destination file to process with ExifTool...";
        return false;
    }

    // ---

    QByteArray wrtCmds;

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Copy Tags Modes:" << writeModes << "(" << QString::fromLatin1("%1").arg(writeModes, 0, 2) << ")";

    if (writeModes & ExifToolProcess::WRITE_EXISTING_TAGS)
    {
        wrtCmds.append(QByteArray("w"));
    }

    if (writeModes & ExifToolProcess::CREATE_NEW_TAGS)
    {
        wrtCmds.append(QByteArray("c"));
    }

    if (writeModes & ExifToolProcess::CREATE_NEW_GROUPS)
    {
        wrtCmds.append(QByteArray("g"));
    }

    if (wrtCmds.isEmpty())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Copy tags writing modes list is empty!";
        return false;
    }

    // ---

    QByteArrayList copyCmds;

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Copy Tags Operations:" << copyOps << "(" << QString::fromLatin1("%1").arg(copyOps, 0, 2) << ")";

    if (!(copyOps & ExifToolProcess::COPY_NONE))
    {
        if (copyOps & ExifToolProcess::COPY_ALL)
        {
            copyCmds << QByteArray("-all:all");
        }
        else
        {
            if (copyOps & ExifToolProcess::COPY_EXIF)
            {
                copyCmds << QByteArray("-exif");
            }
            else
            {
                copyCmds << QByteArray("--exif");
            }

            if (copyOps & ExifToolProcess::COPY_MAKERNOTES)
            {
                copyCmds << QByteArray("-makernotes");
            }
            else
            {
                copyCmds << QByteArray("--makernotes");
            }

            if (copyOps & ExifToolProcess::COPY_IPTC)
            {
                copyCmds << QByteArray("-iptc");
            }
            else
            {
                copyCmds << QByteArray("--iptc");
            }

            if (copyOps & ExifToolProcess::COPY_XMP)
            {
                copyCmds << QByteArray("-xmp");
            }
            else
            {
                copyCmds << QByteArray("--xmp");
            }

            if (copyOps & ExifToolProcess::COPY_ICC)
            {
                copyCmds << QByteArray("-icc_profile");
            }
            else
            {
                copyCmds << QByteArray("--icc_profile");
            }
        }
    }

    // ---

    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    QByteArrayList cmdArgs;

    cmdArgs << QByteArray("-wm") << wrtCmds;
    cmdArgs << QByteArray("-TagsFromFile");
    cmdArgs << d->filePathEncoding(src);
    cmdArgs << copyCmds;
    cmdArgs << QByteArray("-overwrite_original");
    cmdArgs << d->filePathEncoding(dst);
    d->currentPath = sfi.filePath();

    return (d->startProcess(cmdArgs, ExifToolProcess::COPY_TAGS));
}

bool ExifToolParser::translateTags(const QString& path, unsigned char transOps)
{
    QFileInfo fi(path);

    if (!fi.exists())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open source file to process with ExifTool...";
        return false;
    }

    // ---

    if (!d->argsFile.isOpen() && d->argsFile.exists())
    {
        d->argsFile.remove();
    }

    if (!d->argsFile.open())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot open temporary file to write ExifTool tags translate config file...";
        return false;
    }

    QTextStream out(&d->argsFile);
    bool dirty = false;

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Translate Tags:" << transOps << "(" << QString::fromLatin1("%1").arg(transOps, 0, 2) << ")";

    if (transOps & ExifToolProcess::TRANS_ALL_XMP)
    {
        out << QLatin1String("-xmp:all<all:all") << endl;
        dirty = true;
    }

    if (transOps & ExifToolProcess::TRANS_ALL_IPTC)
    {
        out << QLatin1String("-iptc:all<all:all") << endl;
        dirty = true;
    }

    if (transOps & ExifToolProcess::TRANS_ALL_EXIF)
    {
        out << QLatin1String("-exif:all<all:all") << endl;
        dirty = true;
    }

    if (!dirty)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Translate tags operations list is empty!";
        return false;
    }

    // ---

    if (!d->prepareProcess())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "Cannot prepare ExifTool process...";
        return false;
    }

    QByteArrayList cmdArgs;

    cmdArgs << QByteArray("-@") << d->filePathEncoding(d->argsFile.fileName());
    cmdArgs << QByteArray("-overwrite_original");
    cmdArgs << d->filePathEncoding(path);
    d->currentPath = fi.filePath();

    return (d->startProcess(cmdArgs, ExifToolProcess::TRANS_TAGS));
}

} // namespace Digikam
