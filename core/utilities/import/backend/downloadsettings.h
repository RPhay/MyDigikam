/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-21-07
 * Description : Camera item download settings container.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DOWNLOAD_SETTINGS_H
#define DIGIKAM_DOWNLOAD_SETTINGS_H

// Qt includes

#include <QString>
#include <QDateTime>

// Local includes

#include "digikam_globals.h"
#include "dngwriter.h"

namespace Digikam
{

class DownloadSettings
{

public:

    DownloadSettings()
      : autoRotate  (true),
        fixDateTime (false),
        convertJpeg (false),
        documentName(false),
        backupRaw   (false),
        convertDng  (false),
        compressDng (true),
        previewMode (DNGWriter::FULL_SIZE),
        rating      (NoRating),
        pickLabel   (NoPickLabel),
        colorLabel  (NoColorLabel)
    {
    };

    ~DownloadSettings()
    {
    };

public:

    /// Settings from AdvancedSettings widget
    bool       autoRotate;
    bool       fixDateTime;
    bool       convertJpeg;
    bool       documentName;

    QDateTime  newDateTime;

    /// New format to convert Jpeg files.
    QString    losslessFormat;

    /// Metadata template title.
    QString    templateTitle;

    /// File path to download
    QString    folder;
    QString    file;
    QString    dest;

    /// Mime type from file to download
    QString    mime;

    /// Settings from DNG convert widget
    bool       backupRaw;
    bool       convertDng;
    bool       compressDng;
    int        previewMode;

    /// Settings from ScriptingSettings widget
    QString    script;

    /// Pre-rating of each camera file
    int        rating;

    /// Pre-pickLabel of each camera file
    int        pickLabel;

    /// Pre-colorLabel of each camera file
    int        colorLabel;

    /// Pre-tags of each camera file
    QList<int> tagIds;
};

typedef QList<DownloadSettings> DownloadSettingsList;

} // namespace Digikam

#endif // DIGIKAM_DOWNLOAD_SETTINGS_H
