/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image quality Settings Container.
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagequalitycontainer.h"

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_globals.h"

namespace Digikam
{

ImageQualityContainer::ImageQualityContainer()
    : detectBlur        (true),
      detectNoise       (true),
      detectCompression (true),
      detectExposure    (true),
      detectAesthetic   (true),
      lowQRejected      (true),
      mediumQPending    (true),
      highQAccepted     (true),
      rejectedThreshold (10),
      pendingThreshold  (40),
      acceptedThreshold (60),
      blurWeight        (100),
      noiseWeight       (100),
      compressionWeight (100),
      exposureWeight    (100)
{
}

ImageQualityContainer::ImageQualityContainer(const ImageQualityContainer& other)
    : detectBlur        (other.detectBlur),
      detectNoise       (other.detectNoise),
      detectCompression (other.detectCompression),
      detectExposure    (other.detectExposure),
      detectAesthetic   (other.detectAesthetic),
      lowQRejected      (other.lowQRejected),
      mediumQPending    (other.mediumQPending),
      highQAccepted     (other.highQAccepted),
      rejectedThreshold (other.rejectedThreshold),
      pendingThreshold  (other.pendingThreshold),
      acceptedThreshold (other.acceptedThreshold),
      blurWeight        (other.blurWeight),
      noiseWeight       (other.noiseWeight),
      compressionWeight (other.compressionWeight),
      exposureWeight    (other.exposureWeight)
{
}

ImageQualityContainer& ImageQualityContainer::operator=(const ImageQualityContainer& other)
{
    detectBlur         = other.detectBlur;
    detectNoise        = other.detectNoise;
    detectCompression  = other.detectCompression;
    detectExposure     = other.detectExposure;
    detectAesthetic    = other.detectAesthetic;
    lowQRejected       = other.lowQRejected;
    mediumQPending     = other.mediumQPending;
    highQAccepted      = other.highQAccepted;
    rejectedThreshold  = other.rejectedThreshold;
    pendingThreshold   = other.pendingThreshold;
    acceptedThreshold  = other.acceptedThreshold;
    blurWeight         = other.blurWeight;
    noiseWeight        = other.noiseWeight;
    compressionWeight  = other.compressionWeight;
    exposureWeight     = other.exposureWeight;

    return *this;
}

ImageQualityContainer::~ImageQualityContainer()
{
}

void ImageQualityContainer::readFromConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("Image Quality Settings");
    readFromConfig(group);
}

void ImageQualityContainer::readFromConfig(const KConfigGroup& group)
{
    detectBlur                = group.readEntry("Detect Blur",        true);
    detectNoise               = group.readEntry("Detect Noise",       true);
    detectCompression         = group.readEntry("Detect Compression", true);
    detectExposure            = group.readEntry("Detect Exposure",    true);
    detectAesthetic           = group.readEntry("Detect Aesthetic",   true);
    lowQRejected              = group.readEntry("LowQ Rejected",      true);
    mediumQPending            = group.readEntry("MediumQ Pending",    true);
    highQAccepted             = group.readEntry("HighQ Accepted",     true);
    rejectedThreshold         = group.readEntry("Rejected Threshold", 10);
    pendingThreshold          = group.readEntry("Pending Threshold",  40);
    acceptedThreshold         = group.readEntry("Accepted Threshold", 60);
    blurWeight                = group.readEntry("Blur Weight",        100);
    noiseWeight               = group.readEntry("Noise Weight",       100);
    compressionWeight         = group.readEntry("Compression Weight", 100);
    exposureWeight            = group.readEntry("Exposure Weight",    100);
}

void ImageQualityContainer::writeToConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("Image Quality Settings");
    writeToConfig(group);
}

void ImageQualityContainer::writeToConfig(KConfigGroup& group)
{
    group.writeEntry("Detect Blur",         detectBlur);
    group.writeEntry("Detect Noise",        detectNoise);
    group.writeEntry("Detect Compression",  detectCompression);
    group.writeEntry("Detect Exposure",     detectExposure);
    group.writeEntry("Detect Aesthetic",    detectAesthetic);
    group.writeEntry("LowQ Rejected",       lowQRejected);
    group.writeEntry("MediumQ Pending",     mediumQPending);
    group.writeEntry("HighQ Accepted",      highQAccepted);
    group.writeEntry("Rejected Threshold",  rejectedThreshold);
    group.writeEntry("Pending Threshold",   pendingThreshold);
    group.writeEntry("Accepted Threshold",  acceptedThreshold);
    group.writeEntry("Blur Weight",         blurWeight);
    group.writeEntry("Noise Weight",        noiseWeight);
    group.writeEntry("Compression Weight",  compressionWeight);
    group.writeEntry("Exposure Weight",     exposureWeight);
}

QDebug operator<<(QDebug dbg, const ImageQualityContainer& s)
{
    dbg.nospace()                                                   << QT_ENDL;
    dbg.nospace() << "DetectBlur         :" << s.detectBlur         << QT_ENDL;
    dbg.nospace() << "DetectNoise        :" << s.detectNoise        << QT_ENDL;
    dbg.nospace() << "DetectCompression  :" << s.detectCompression  << QT_ENDL;
    dbg.nospace() << "DetectExposure     :" << s.detectExposure     << QT_ENDL;
    dbg.nospace() << "DetectAesthetic    :" << s.detectAesthetic    << QT_ENDL;
    dbg.nospace() << "LowQRejected       :" << s.lowQRejected       << QT_ENDL;
    dbg.nospace() << "MediumQPending     :" << s.mediumQPending     << QT_ENDL;
    dbg.nospace() << "HighQAccepted      :" << s.highQAccepted      << QT_ENDL;
    dbg.nospace() << "Rejected Threshold :" << s.rejectedThreshold  << QT_ENDL;
    dbg.nospace() << "Pending Threshold  :" << s.pendingThreshold   << QT_ENDL;
    dbg.nospace() << "Accepted Threshold :" << s.acceptedThreshold  << QT_ENDL;
    dbg.nospace() << "Blur Weight        :" << s.blurWeight         << QT_ENDL;
    dbg.nospace() << "Noise Weight       :" << s.noiseWeight        << QT_ENDL;
    dbg.nospace() << "Compression Weight :" << s.compressionWeight  << QT_ENDL;
    dbg.nospace() << "Exposure Weight    :" << s.exposureWeight     << QT_ENDL;

    return dbg.space();
}

} // namespace Digikam
