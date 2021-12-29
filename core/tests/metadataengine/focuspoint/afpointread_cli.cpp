/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : a command line tool to read focus points metadata with ExifTool
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
        int id = 1;

        foreach (const FocusPoint& fp, points)
        {
            qCDebug(DIGIKAM_TESTS_LOG) << id << "AF Focus region found in" << argv[1] << ":" << fp;
            qCDebug(DIGIKAM_TESTS_LOG) << id << "AF Focus coordinates in image" << argv[1] << ":" << fp.getRectBySize(fpreader->originalSize());
            ++id;
        }
    }
    else
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "No AF Focus region found in" << argv[1];
    }

    delete fpreader;

    return 0;
}
