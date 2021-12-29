/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : a command line tool to detect image quality
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDebug>

// Local includes

#include "imgqsorttest_shared.h"
#include "digikam_globals.h"
#include "dpluginloader.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qDebug() << "detect all quality - Parse image data to detect general level";
        qDebug() << "Usage: <image file>";
        return -1;
    }

    QApplication app(argc, argv);

    DPluginLoader::instance()->init();

    QString path                = QString::fromUtf8(argv[1]);
    QFileInfoList list          = QFileInfoList() << QFileInfo(path);
    ImgQSortTest_ParseTestImagesDefautDetection(DETECTIONGENERAL, list);

    return 0;
}
