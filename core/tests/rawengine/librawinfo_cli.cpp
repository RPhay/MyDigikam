/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-09-07
 * Description : a command line tool to show RawEngine info
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QString>

// Local includes

#include "drawdecoder.h"
#include "digikam_debug.h"

using namespace Digikam;

int main(int /*argc*/, char** /*argv*/)
{
    qCDebug(DIGIKAM_TESTS_LOG) << "Libraw version    : " << DRawDecoder::librawVersion();
    qCDebug(DIGIKAM_TESTS_LOG) << "Use OpenMP        : " << DRawDecoder::librawUseGomp();
    qCDebug(DIGIKAM_TESTS_LOG) << "Raw files list    : " << DRawDecoder::rawFilesList();
    qCDebug(DIGIKAM_TESTS_LOG) << "Raw files version : " << DRawDecoder::rawFilesVersion();
    qCDebug(DIGIKAM_TESTS_LOG) << "Supported camera  : " << DRawDecoder::supportedCamera();

    return 0;
}
