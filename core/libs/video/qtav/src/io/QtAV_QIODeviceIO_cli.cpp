/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#include "MediaIO.h"
#include "private/MediaIO_p.h"
#include "private/mkid.h"
#include "private/factory.h"

#include <QFile>
#include <QtDebug>
#include <QApplication>

int main(int, char**)
{
    QApplication a(argc, argv);

    if (argc == 1)
    {
        qCDebug(DIGIKAM_QTAV_LOG) << "QtAV test QIODevice IO - media file to stream";
        qCDebug(DIGIKAM_QTAV_LOG) << "Usage: media files";

        return -1;
    }

    QtAV::QFileIO fi;
    qCDebug(DIGIKAM_QTAV_LOG) << "protocols: " << fi.protocols();
    fi.setUrl(QString::fromUtf8(argv[1]));
    QByteArray data(1024, 0);
    fi.read(data.data(), data.size());
    qDebug("QFileIO url: %s, seekable: %d, size: %lld", fi.url().toUtf8().constData(), fi.isSeekable(), fi.size());
    qCDebug(DIGIKAM_QTAV_LOG) << data;

    return 0;
}
