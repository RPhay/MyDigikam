/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-04-19
 * Description : time adjust settings container.
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TIME_ADJUST_CONTAINER_H
#define DIGIKAM_TIME_ADJUST_CONTAINER_H

// Qt includes

#include <QUrl>
#include <QMap>
#include <QDateTime>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

/**
 * Container that store all timestamp adjustments.
 */
class DIGIKAM_EXPORT TimeAdjustContainer
{

public:

    enum UseDateSource
    {
        APPDATE = 0,
        FILENAME,
        FILEDATE,
        METADATADATE,
        CUSTOMDATE
    };

    enum UseMetaDateType
    {
        EXIFIPTCXMP = 0,
        EXIFCREATED,
        EXIFORIGINAL,
        EXIFDIGITIZED,
        IPTCCREATED,
        XMPCREATED,
        FUZZYCREATED,
        FUZZYORIGINAL,
        FUZZYDIGITIZED
    };

    enum UseFileDateType
    {
        FILELASTMOD = 0,
        FILECREATED
    };

    enum AdjType
    {
        COPYVALUE = 0,
        ADDVALUE,
        SUBVALUE,
        INTERVAL
    };

public:

    TimeAdjustContainer();
    ~TimeAdjustContainer();

    /// Check if at least one option is selected
    bool atLeastOneUpdateToProcess()                                const;

    QDateTime calculateAdjustedDate(const QDateTime& originalTime, int index = 0);
    QDateTime getDateTimeFromString(const QString& dateStr)         const;
    QMap<QString, bool> getDateTimeTagsMap()                        const;

public:

    QDateTime customDate;
    QDateTime customTime;
    QDateTime adjustmentTime;

    bool      updIfAvailable;
    bool      updEXIFModDate;
    bool      updEXIFOriDate;
    bool      updEXIFDigDate;
    bool      updEXIFThmDate;
    bool      updIPTCDate;
    bool      updXMPVideo;
    bool      updXMPDate;
    bool      updFileModDate;

    int       dateSource;
    int       metadataSource;
    int       fileDateSource;
    int       adjustmentType;
    int       adjustmentDays;
};

// -------------------------------------------------------------------

/**
 * Container that hold the time difference for clock photo dialog.
 */
class DeltaTime
{

public:

    explicit DeltaTime();

    ~DeltaTime();

    /// Check if at least one option is selected
    bool isNull() const;

public:

    bool deltaNegative;

    int  deltaDays;
    int  deltaHours;
    int  deltaMinutes;
    int  deltaSeconds;
};

} // namespace Digikam

#endif // DIGIKAM_TIME_ADJUST_CONTAINER_H
