/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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


#include <cstdio>
#include <cstdlib>
#include <QCoreApplication>
#include <QDateTime>
#include <QQueue>
#include <QStringList>
#include <QtAV/FrameReader.h>
using namespace QtAV;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FrameReader r;
    r.setMedia(a.arguments().last());
    QQueue<qint64> t;
    int count = 0;
    qint64 t0 = QDateTime::currentMSecsSinceEpoch();
    while (r.readMore()) {
        while (r.hasEnoughVideoFrames()) {
            const VideoFrame f = r.getVideoFrame(); //TODO: if eof
            if (!f)
                continue;
            count++;
            //r.readMore();
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const qint64 dt = now - t0;
            t.enqueue(now);
            printf("decode @%.3f count: %d, elapsed: %lld, fps: %.1f/%.1f\r", f.timestamp(), count, dt, count*1000.0/dt, t.size()*1000.0/(now - t.first()));fflush(0);
            if (t.size() > 10)
                t.dequeue();
        }
    }
    while (r.hasVideoFrame()) {
        const VideoFrame f = r.getVideoFrame();
        qDebug("pts: %.3f", f.timestamp());
    }
    qDebug("read done");
    return 0;
}

