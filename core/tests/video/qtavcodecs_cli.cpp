/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to show QtAV codecs available.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include <QPainterPath>

// QtAv includes

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wundef"
#endif

#include <QtAV/VideoDecoder.h> // krazy:exclude=includes
#include <QtAV/VideoEncoder.h> // krazy:exclude=includes
#include <QtAV/AudioDecoder.h> // krazy:exclude=includes
#include <QtAV/AudioEncoder.h> // krazy:exclude=includes
#include <QtAV/AVMuxer.h>      // krazy:exclude=includes
#include <QtAV/LibAVFilter.h>  // krazy:exclude=includes

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

// Local includes

#include "digikam_debug.h"

using namespace QtAV;

int main(int /*argc*/, char** /*argv*/)
{
    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Video Decoder Codecs ---";
    qCDebug(DIGIKAM_TESTS_LOG) << VideoDecoder::supportedCodecs();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Audio Decoder Codecs ---";
    qCDebug(DIGIKAM_TESTS_LOG) << AudioDecoder::supportedCodecs();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Video Encoder Codecs ---";
    qCDebug(DIGIKAM_TESTS_LOG) << VideoEncoder::supportedCodecs();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Audio Encoder Codecs ---";
    qCDebug(DIGIKAM_TESTS_LOG) << AudioEncoder::supportedCodecs();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Media Formats ---";
    qCDebug(DIGIKAM_TESTS_LOG) << AVMuxer::supportedFormats();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Media Extensions ---";
    qCDebug(DIGIKAM_TESTS_LOG) << AVMuxer::supportedExtensions();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Video Filters ---";
    qCDebug(DIGIKAM_TESTS_LOG) << LibAVFilter::videoFilters();

    qCDebug(DIGIKAM_TESTS_LOG) << "--- Available Audio Filters ---";
    qCDebug(DIGIKAM_TESTS_LOG) << LibAVFilter::audioFilters();

    return 0;
}
