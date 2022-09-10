/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to show QtAV codecs available.
 *
 * Copyright (C) 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QPainterPath>

// Local includes

#include "VideoDecoder.h"
#include "VideoEncoder.h"
#include "AudioDecoder.h"
#include "AudioEncoder.h"
#include "AVMuxer.h"
#include "LibAVFilter.h"

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
