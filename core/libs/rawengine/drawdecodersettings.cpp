/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-09
 * Description : Raw decoding settings
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2008 by Guillaume Castagnino <casta at xwing dot info>
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

#include "drawdecodersettings.h"

// Local includes

#include "digikam_globals.h"

namespace Digikam
{

DRawDecoderSettings::DRawDecoderSettings()
    : fixColorsHighlights       (false),
      autoBrightness            (true),
      sixteenBitsImage          (false),
      halfSizeColorImage        (false),
      whiteBalance              (CAMERA),
      customWhiteBalance        (6500),
      customWhiteBalanceGreen   (1.0),
      RGBInterpolate4Colors     (false),
      DontStretchPixels         (false),
      unclipColors              (0),
      RAWQuality                (BILINEAR),
      medianFilterPasses        (0),
      NRType                    (NONR),
      NRThreshold               (0),
      brightness                (1.0),
      enableBlackPoint          (false),
      blackPoint                (0),
      enableWhitePoint          (false),
      whitePoint                (0),
      inputColorSpace           (NOINPUTCS),
      inputProfile              (QString()),
      outputColorSpace          (SRGB),
      outputProfile             (QString()),
      deadPixelMap              (QString()),
      whiteBalanceArea          (QRect()),

      //-- Extended demosaicing settings ----------------------------------------------------------

      dcbIterations             (-1),
      dcbEnhanceFl              (false),
      expoCorrection            (false),
      expoCorrectionShift       (1.0),
      expoCorrectionHighlight   (0.0)
{
}

DRawDecoderSettings::~DRawDecoderSettings()
{
}

DRawDecoderSettings::DRawDecoderSettings(const DRawDecoderSettings& o)
    : fixColorsHighlights       (o.fixColorsHighlights),
      autoBrightness            (o.autoBrightness),
      sixteenBitsImage          (o.sixteenBitsImage),
      halfSizeColorImage        (o.halfSizeColorImage),
      whiteBalance              (o.whiteBalance),
      customWhiteBalance        (o.customWhiteBalance),
      customWhiteBalanceGreen   (o.customWhiteBalanceGreen),
      RGBInterpolate4Colors     (o.RGBInterpolate4Colors),
      DontStretchPixels         (o.DontStretchPixels),
      unclipColors              (o.unclipColors),
      RAWQuality                (o.RAWQuality),
      medianFilterPasses        (o.medianFilterPasses),
      NRType                    (o.NRType),
      NRThreshold               (o.NRThreshold),
      brightness                (o.brightness),
      enableBlackPoint          (o.enableBlackPoint),
      blackPoint                (o.blackPoint),
      enableWhitePoint          (o.enableWhitePoint),
      whitePoint                (o.whitePoint),
      inputColorSpace           (o.inputColorSpace),
      inputProfile              (o.inputProfile),
      outputColorSpace          (o.outputColorSpace),
      outputProfile             (o.outputProfile),
      deadPixelMap              (o.deadPixelMap),
      whiteBalanceArea          (o.whiteBalanceArea),

      //-- Extended demosaicing settings ----------------------------------------------------------

      dcbIterations             (o.dcbIterations),
      dcbEnhanceFl              (o.dcbEnhanceFl),
      expoCorrection            (o.expoCorrection),
      expoCorrectionShift       (o.expoCorrectionShift),
      expoCorrectionHighlight   (o.expoCorrectionHighlight)
{
}

DRawDecoderSettings& DRawDecoderSettings::operator=(const DRawDecoderSettings& o)
{
    fixColorsHighlights     = o.fixColorsHighlights;
    autoBrightness          = o.autoBrightness;
    sixteenBitsImage        = o.sixteenBitsImage;
    brightness              = o.brightness;
    RAWQuality              = o.RAWQuality;
    inputColorSpace         = o.inputColorSpace;
    outputColorSpace        = o.outputColorSpace;
    RGBInterpolate4Colors   = o.RGBInterpolate4Colors;
    DontStretchPixels       = o.DontStretchPixels;
    unclipColors            = o.unclipColors;
    whiteBalance            = o.whiteBalance;
    customWhiteBalance      = o.customWhiteBalance;
    customWhiteBalanceGreen = o.customWhiteBalanceGreen;
    halfSizeColorImage      = o.halfSizeColorImage;
    enableBlackPoint        = o.enableBlackPoint;
    blackPoint              = o.blackPoint;
    enableWhitePoint        = o.enableWhitePoint;
    whitePoint              = o.whitePoint;
    NRType                  = o.NRType;
    NRThreshold             = o.NRThreshold;
    medianFilterPasses      = o.medianFilterPasses;
    inputProfile            = o.inputProfile;
    outputProfile           = o.outputProfile;
    deadPixelMap            = o.deadPixelMap;
    whiteBalanceArea        = o.whiteBalanceArea;

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations           = o.dcbIterations;
    dcbEnhanceFl            = o.dcbEnhanceFl;
    expoCorrection          = o.expoCorrection;
    expoCorrectionShift     = o.expoCorrectionShift;
    expoCorrectionHighlight = o.expoCorrectionHighlight;

    return *this;
}

bool DRawDecoderSettings::operator==(const DRawDecoderSettings& o) const
{
    return (
            (fixColorsHighlights     == o.fixColorsHighlights)      &&
            (autoBrightness          == o.autoBrightness)           &&
            (sixteenBitsImage        == o.sixteenBitsImage)         &&
            (brightness              == o.brightness)               &&
            (RAWQuality              == o.RAWQuality)               &&
            (inputColorSpace         == o.inputColorSpace)          &&
            (outputColorSpace        == o.outputColorSpace)         &&
            (RGBInterpolate4Colors   == o.RGBInterpolate4Colors)    &&
            (DontStretchPixels       == o.DontStretchPixels)        &&
            (unclipColors            == o.unclipColors)             &&
            (whiteBalance            == o.whiteBalance)             &&
            (customWhiteBalance      == o.customWhiteBalance)       &&
            (customWhiteBalanceGreen == o.customWhiteBalanceGreen)  &&
            (halfSizeColorImage      == o.halfSizeColorImage)       &&
            (enableBlackPoint        == o.enableBlackPoint)         &&
            (blackPoint              == o.blackPoint)               &&
            (enableWhitePoint        == o.enableWhitePoint)         &&
            (whitePoint              == o.whitePoint)               &&
            (NRType                  == o.NRType)                   &&
            (NRThreshold             == o.NRThreshold)              &&
            (medianFilterPasses      == o.medianFilterPasses)       &&
            (inputProfile            == o.inputProfile)             &&
            (outputProfile           == o.outputProfile)            &&
            (deadPixelMap            == o.deadPixelMap)             &&
            (whiteBalanceArea        == o.whiteBalanceArea)         &&

            //-- Extended demosaicing settings ----------------------------------------------------------

            (dcbIterations           == o.dcbIterations)            &&
            (dcbEnhanceFl            == o.dcbEnhanceFl)             &&
            (expoCorrection          == o.expoCorrection)           &&
            (expoCorrectionShift     == o.expoCorrectionShift)      &&
            (expoCorrectionHighlight == o.expoCorrectionHighlight)
           );
}

void DRawDecoderSettings::optimizeTimeLoading()
{
    fixColorsHighlights     = false;
    autoBrightness          = true;
    sixteenBitsImage        = true;
    brightness              = 1.0;
    RAWQuality              = BILINEAR;
    inputColorSpace         = NOINPUTCS;
    outputColorSpace        = SRGB;
    RGBInterpolate4Colors   = false;
    DontStretchPixels       = false;
    unclipColors            = 0;
    whiteBalance            = CAMERA;
    customWhiteBalance      = 6500;
    customWhiteBalanceGreen = 1.0;
    halfSizeColorImage      = true;
    medianFilterPasses      = 0;

    enableBlackPoint        = false;
    blackPoint              = 0;

    enableWhitePoint        = false;
    whitePoint              = 0;

    NRType                  = NONR;
    NRThreshold             = 0;

    inputProfile            = QString();
    outputProfile           = QString();

    deadPixelMap            = QString();

    whiteBalanceArea        = QRect();

    //-- Extended demosaicing settings ----------------------------------------------------------

    dcbIterations           = -1;
    dcbEnhanceFl            = false;
    expoCorrection          = false;
    expoCorrectionShift     = 1.0;
    expoCorrectionHighlight = 0.0;
}

QDebug operator<<(QDebug dbg, const DRawDecoderSettings& s)
{
    dbg.nospace() << QT_ENDL;
    dbg.nospace() << "-- RAW DECODING SETTINGS --------------------------------" << QT_ENDL;
    dbg.nospace() << "-- autoBrightness:          " << s.autoBrightness          << QT_ENDL;
    dbg.nospace() << "-- sixteenBitsImage:        " << s.sixteenBitsImage        << QT_ENDL;
    dbg.nospace() << "-- brightness:              " << s.brightness              << QT_ENDL;
    dbg.nospace() << "-- RAWQuality:              " << s.RAWQuality              << QT_ENDL;
    dbg.nospace() << "-- inputColorSpace:         " << s.inputColorSpace         << QT_ENDL;
    dbg.nospace() << "-- outputColorSpace:        " << s.outputColorSpace        << QT_ENDL;
    dbg.nospace() << "-- RGBInterpolate4Colors:   " << s.RGBInterpolate4Colors   << QT_ENDL;
    dbg.nospace() << "-- DontStretchPixels:       " << s.DontStretchPixels       << QT_ENDL;
    dbg.nospace() << "-- unclipColors:            " << s.unclipColors            << QT_ENDL;
    dbg.nospace() << "-- whiteBalance:            " << s.whiteBalance            << QT_ENDL;
    dbg.nospace() << "-- customWhiteBalance:      " << s.customWhiteBalance      << QT_ENDL;
    dbg.nospace() << "-- customWhiteBalanceGreen: " << s.customWhiteBalanceGreen << QT_ENDL;
    dbg.nospace() << "-- halfSizeColorImage:      " << s.halfSizeColorImage      << QT_ENDL;
    dbg.nospace() << "-- enableBlackPoint:        " << s.enableBlackPoint        << QT_ENDL;
    dbg.nospace() << "-- blackPoint:              " << s.blackPoint              << QT_ENDL;
    dbg.nospace() << "-- enableWhitePoint:        " << s.enableWhitePoint        << QT_ENDL;
    dbg.nospace() << "-- whitePoint:              " << s.whitePoint              << QT_ENDL;
    dbg.nospace() << "-- NoiseReductionType:      " << s.NRType                  << QT_ENDL;
    dbg.nospace() << "-- NoiseReductionThreshold: " << s.NRThreshold             << QT_ENDL;
    dbg.nospace() << "-- medianFilterPasses:      " << s.medianFilterPasses      << QT_ENDL;
    dbg.nospace() << "-- inputProfile:            " << s.inputProfile            << QT_ENDL;
    dbg.nospace() << "-- outputProfile:           " << s.outputProfile           << QT_ENDL;
    dbg.nospace() << "-- deadPixelMap:            " << s.deadPixelMap            << QT_ENDL;
    dbg.nospace() << "-- whiteBalanceArea:        " << s.whiteBalanceArea        << QT_ENDL;

    //-- Extended demosaicing settings ----------------------------------------------------------

    dbg.nospace() << "-- dcbIterations:           " << s.dcbIterations           << QT_ENDL;
    dbg.nospace() << "-- dcbEnhanceFl:            " << s.dcbEnhanceFl            << QT_ENDL;
    dbg.nospace() << "-- expoCorrection:          " << s.expoCorrection          << QT_ENDL;
    dbg.nospace() << "-- expoCorrectionShift:     " << s.expoCorrectionShift     << QT_ENDL;
    dbg.nospace() << "-- expoCorrectionHighlight: " << s.expoCorrectionHighlight << QT_ENDL;
    dbg.nospace() << "---------------------------------------------------------" << QT_ENDL;

    return dbg.space();
}

} // namespace Digikam
