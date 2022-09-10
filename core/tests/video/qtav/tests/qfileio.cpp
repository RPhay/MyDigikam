/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QFile>
#include <QApplication>

// Local includes

#include "MediaIO_p.h"
#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

int main(int argc, char** argv)
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

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("QFileIO url: %s, seekable: %d, size: %lld",
            fi.url().toUtf8().constData(), fi.isSeekable(), fi.size());

    qCDebug(DIGIKAM_QTAV_LOG) << data;

    return 0;
}
