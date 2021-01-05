/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : an unit test to download version online.
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

#include <QApplication>
#include <QTest>
#include <QDebug>
#include <QObject>

// Local includes

#include "onlineversiondwnl.h"

using namespace Digikam;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    OnlineVersionDwnl* const dwnl = new OnlineVersionDwnl;
    dwnl->startDownload(QLatin1String("7.1.0"));

    QObject::connect(dwnl, &Digikam::OnlineVersionDwnl::signalDownloadProgress,
                     [=](qint64 brecv, qint64 btotal)
        {
            if (btotal)
            {
                qDebug() << "Downloaded" << brecv << "/" << btotal << "bytes" << "(" << brecv*100/btotal << "% )";
            }
        }
    );

    app.exec();

    return 0;
}
