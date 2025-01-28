/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image quality Settings Container.
 *
 * SPDX-FileCopyrightText: 2013-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QDebug>

// Local includes

#include "digikam_export.h"
#include "album.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_GUI_EXPORT ImageQualityContainer
{
public:

    ImageQualityContainer()  = default;
    ImageQualityContainer(const ImageQualityContainer& other);
    ~ImageQualityContainer() = default;

    ImageQualityContainer& operator=(const ImageQualityContainer& other);

public:

    void readFromConfig();
    void readFromConfig(const KConfigGroup&);
    void writeToConfig();
    void writeToConfig(KConfigGroup&);

public:

    bool      wholeAlbums       = true;     ///< Processing power.
    AlbumList albums;                       ///< Albums to scan.

    bool      detectBlur        = true;     ///< Enable image blur detection.
    bool      detectNoise       = true;     ///< Enable image noise detection.
    bool      detectCompression = true;     ///< Enable image compression detection.
    bool      detectExposure    = true;     ///< Enable image over and under exposure detection.
    bool      detectAesthetic   = true;     ///< Enable image aesthetic detection.

    bool      lowQRejected      = true;     ///< Assign Rejected property to low quality.
    bool      mediumQPending    = true;     ///< Assign Pending property to medium quality.
    bool      highQAccepted     = true;     ///< Assign Accepted property to high quality.

    int       rejectedThreshold = 10;       ///< Item rejection threshold.
    int       pendingThreshold  = 40;       ///< Item pending threshold.
    int       acceptedThreshold = 60;       ///< Item accepted threshold.
    int       blurWeight        = 100;      ///< Item blur level.
    int       noiseWeight       = 100;      ///< Item noise level.
    int       compressionWeight = 100;      ///< Item compression level.
    int       exposureWeight    = 100;      ///< Item exposure level.
};

//! qDebug() stream operator. Writes property @a s to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const ImageQualityContainer& s);

} // namespace Digikam
