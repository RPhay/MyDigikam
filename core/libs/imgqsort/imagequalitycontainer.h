/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image quality Settings Container.
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT ImageQualityContainer
{
public:

    ImageQualityContainer();
    ImageQualityContainer(const ImageQualityContainer& other);
    ~ImageQualityContainer();

    ImageQualityContainer& operator=(const ImageQualityContainer& other);

public:

    void readFromConfig();
    void readFromConfig(const KConfigGroup&);
    void writeToConfig();
    void writeToConfig(KConfigGroup&);

public:

    bool detectBlur;            ///< Enable image blur detection.
    bool detectNoise;           ///< Enable image noise detection.
    bool detectCompression;     ///< Enable image compression detection.
    bool detectExposure;        ///< Enable image over and under exposure detection.
    bool detectAesthetic;       ///< Enable image aesthetic detection.

    bool lowQRejected;          ///< Assign Rejected property to low quality.
    bool mediumQPending;        ///< Assign Pending property to medium quality.
    bool highQAccepted;         ///< Assign Accepted property to high quality.

    int  rejectedThreshold;     ///< Item rejection threshold.
    int  pendingThreshold;      ///< Item pending threshold.
    int  acceptedThreshold;     ///< Item accepted threshold.
    int  blurWeight;            ///< Item blur level.
    int  noiseWeight;           ///< Item noise level.
    int  compressionWeight;     ///< Item compression level.
    int  exposureWeight;        ///< Item exposure level.
};

//! qDebug() stream operator. Writes property @a s to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const ImageQualityContainer& s);

} // namespace Digikam
