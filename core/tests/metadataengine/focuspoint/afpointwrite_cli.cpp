/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : a command line tool to write focus points metadata with ExifTool
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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
#include <QCoreApplication>

// Local includes

#include "digikam_debug.h"
#include "focuspoints_writer.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "afpointwrite - CLI tool to write focus points metadata with ExifTool in image";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image to write>";
        return -1;
    }

    FocusPointsWriter* const fpwriter = new FocusPointsWriter(qApp, QString::fromUtf8(argv[1]));

    FocusPoint point =
    {
        0.5,
        0.5,
        0.5,
        0.5
    };

    fpwriter->writeFocusPoint(point);

    delete fpwriter;

    return 0;
}
