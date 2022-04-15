/******************************************************************************
    QtAV:  Multimedia framework based on Qt and FFmpeg
    Copyright (C) 2012-2016 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV (from 2014)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#include "QtAV/MediaIO.h"
#include "QtAV/private/MediaIO_p.h"
#include "QtAV/private/mkid.h"
#include "QtAV/private/factory.h"

#include <QtCore/QFile>
#include <QtDebug>
#include <QApplication>

int main(int, char**)
{
    QApplication a(argc, argv);

    if (argc == 1)
    {
        qDebug() << "QtAV test QIODevice IO - media file to stream";
        qDebug() << "Usage: media files";

        return -1;
    }

    QtAV::QFileIO fi;
    qDebug() << "protocols: " << fi.protocols();
    fi.setUrl(QString::fromUtf8(argv[1]));
    QByteArray data(1024, 0);
    fi.read(data.data(), data.size());
    qDebug("QFileIO url: %s, seekable: %d, size: %lld", fi.url().toUtf8().constData(), fi.isSeekable(), fi.size());
    qDebug() << data;

    return 0;
}
