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

// C++ includes

#include <cstdio>
#include <cstdlib>

// Qt includes

#include <QCoreApplication>
#include <QDateTime>
#include <QQueue>
#include <QStringList>

// Local includes

#include "FrameReader.h"
#include "digikam_debug.h"

using namespace QtAV;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FrameReader r;
    r.setMedia(a.arguments().last());
    QQueue<qint64> t;
    int count = 0;
    qint64 t0 = QDateTime::currentMSecsSinceEpoch();

    while (r.readMore())
    {
        while (r.hasEnoughVideoFrames())
        {
            const VideoFrame f = r.getVideoFrame(); // TODO: if eof

            if (!f)
                continue;

            count++;

            //r.readMore();

            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const qint64 dt  = now - t0;
            t.enqueue(now);

            qCDebug(DIGIKAM_TESTS_LOG) << "decode @"
                                       << f.timestamp()
                                       << "count:"
                                       << count
                                       << ", elapsed:"
                                       << dt
                                       << ", fps:"
                                       << count * 1000.0 / dt
                                       << "/"
                                       << t.size() * 1000.0 / (now - t.first());

            if (t.size() > 10)
                t.dequeue();
        }
    }

    while (r.hasVideoFrame())
    {
        const VideoFrame f = r.getVideoFrame();

        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("pts: %.3f",
                f.timestamp());
    }

    qCDebug(DIGIKAM_TESTS_LOG).noquote()
        << QString::asprintf("read done");

    return 0;
}
