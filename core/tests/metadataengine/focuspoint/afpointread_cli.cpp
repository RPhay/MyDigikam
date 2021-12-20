/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : a command line tool to read focus points metadata with ExifTool
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "focuspoints_extractor.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "afpointread - CLI tool to read focus points metadata with ExifTool from image";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image to read>";
        return -1;
    }

    FocusPointsExtractor* const fpreader      = new FocusPointsExtractor(qApp, QString::fromUtf8(argv[1]));
    FocusPointsExtractor::ListAFPoints points = fpreader->get_af_points();

    qCDebug(DIGIKAM_TESTS_LOG) << "Make/Model from" << argv[1] << ":" << fpreader->make() << "/" << fpreader->model();
    qCDebug(DIGIKAM_TESTS_LOG) << "Original Image Size:" << fpreader->originalSize();

    if (!points.isEmpty())
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "AF Focus region found in" << argv[1] << ":" << points;
    }
    else
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "No AF Focus region found in" << argv[1];
    }

    delete fpreader;

    return 0;
}
