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

#include <QCoreApplication>
#include <QDateTime>
#include <QQueue>
#include <QStringList>

// Local includes

#include "AVDemuxer.h"
#include "VideoDecoder.h"
#include "Packet.h"
#include "digikam_debug.h"

using namespace QtAV;

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    QString file = QString::fromLatin1("test.avi");

    int idx = a.arguments().indexOf(QLatin1String("-f"));

    if (idx > 0)
        file = a.arguments().at(idx + 1);

    QString decName = QString::fromLatin1("FFmpeg");
    idx             = a.arguments().indexOf(QLatin1String("-vc"));

    if (idx < 0)
        idx = a.arguments().indexOf(QLatin1String("-vd"));

    if (idx > 0)
        decName = a.arguments().at(idx + 1);

    QString opt;
    QVariantHash decopt;
    idx = decName.indexOf(QLatin1String(":"));

    if (idx > 0)
    {
        opt     = decName.right(decName.size() - idx - 1);
        decName = decName.left(idx);
        QStringList opts(opt.split(QString::fromLatin1(";")));
        QVariantHash subopt;

        Q_FOREACH (const QString& o, opts)
        {
            idx                 = o.indexOf(QLatin1String(":"));
            subopt[o.left(idx)] = o.right(o.size() - idx - 1);
        }

        decopt[decName] = subopt;
    }

    qCDebug(DIGIKAM_TESTS_LOG) << decopt;

    VideoDecoder* const dec = VideoDecoder::create(decName.toLatin1().constData());

    if (!dec)
    {
        qCWarning(DIGIKAM_TESTS_LOG)
            << "Can not find decoder:"
                << decName.toUtf8().constData();

        return 1;
    }

    if (!decopt.isEmpty())
        dec->setOptions(decopt);

    AVDemuxer demux;
    demux.setMedia(file);

    if (!demux.load())
    {
        qCCritical(DIGIKAM_TESTS_LOG)
            << QString::asprintf("Failed to load file: %s",
                file.toUtf8().constData());

        return 1;
    }

    dec->setCodecContext(demux.videoCodecContext());
    dec->open();
    int count   = 0;
    int vstream = demux.videoStream();
    QQueue<qint64> t;
    qint64 t0   = QDateTime::currentMSecsSinceEpoch();

    while (!demux.atEnd())
    {
        if (!demux.readPacket())
            continue;

        if (demux.stream() != vstream)
            continue;

        const Packet pkt = demux.packet();

        if (dec->decode(pkt))
        {
            // why is faster to call frame() for hwdec? no frame() is very slow for VDA

            VideoFrame frame = dec->frame();
            Q_UNUSED(frame);

            count++;
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const qint64 dt  = now - t0;
            t.enqueue(now);

            qCDebug(DIGIKAM_TESTS_LOG) << "decode count:"
                << count
                << "elapsed:"
                << dt
                << ", fps:"
                << count * 1000.0 / dt
                << "/"
                << t.size() * 1000.0 / (now - t.first());

            if (t.size() > 10)
                t.dequeue();
        }
    }

    return 0;
}
