/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-02-17
 * Description : a command line tool to convert RAW file to DNG
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QCoreApplication>

// Local includes

#include "dngwriter.h"
#include "digikam_debug.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "raw2dng - RAW Camera Image to DNG Converter";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <rawfile>";

        return -1;
    }

    Digikam::DNGWriter dngProcessor;
    dngProcessor.setInputFile(QString::fromUtf8(argv[1]));
    int ret = dngProcessor.convert();

    return ret;
}
