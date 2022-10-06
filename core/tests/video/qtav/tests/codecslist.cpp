/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to show QtAV codecs available.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <iostream>
#include "digikam_debug.h"

using namespace QtAV;

void printList(const char* header, const QStringList& list)
{
    std::cout << header << "\n";
    for(auto item : list)
    {
        std::cout << "  " << item.toUtf8().constData() << ", ";
    }
    std::cout << "\n\n";
}


int main(int /*argc*/, char** /*argv*/)
{
    printList("--- Available Video Decoder Codecs ---", VideoDecoder::supportedCodecs());
    printList("--- Available Audio Decoder Codecs ---", AudioDecoder::supportedCodecs());
    
    printList("--- Available Video Encoder Codecs ---", VideoEncoder::supportedCodecs());
    printList("--- Available Audio Encoder Codecs ---", AudioEncoder::supportedCodecs());

    printList("--- Available Media Formats ---", AVMuxer::supportedFormats());
    printList("--- Available Media Extensions ---", AVMuxer::supportedExtensions());

    printList("--- Available Video Filters ---", LibAVFilter::videoFilters());
    printList("--- Available Audio Filters ---", LibAVFilter::audioFilters());

    return 0;
}
