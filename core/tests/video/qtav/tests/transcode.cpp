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
#include <QDir>
#include <QElapsedTimer>
#include <QStringList>

// Local includes

#include "QtAV.h"
#include "VideoEncoder.h"
#include "AVMuxer.h"
#include "digikam_debug.h"

using namespace QtAV;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString file = QString::fromLatin1("test.avi");
    int idx      = a.arguments().indexOf(QLatin1String("-i"));

    if (idx > 0)
        file = a.arguments().at(idx + 1);

    QString decName = QString::fromLatin1("FFmpeg");
    idx             = a.arguments().indexOf(QLatin1String("-d:v"));

    if (idx > 0)
        decName = a.arguments().at(idx + 1);

    QString outFile = QString::fromLatin1("/tmp/out.mp4");
    idx             = a.arguments().indexOf(QLatin1String("-o"));

    if (idx > 0)
        outFile = a.arguments().at(idx + 1);

    QDir().mkpath(outFile.left(outFile.lastIndexOf(QLatin1Char('/')) + 1));

    QString cv = QString::fromLatin1("libx264");
    idx        = a.arguments().indexOf(QLatin1String("-c:v"));

    if (idx > 0)
        cv = a.arguments().at(idx + 1);

    QString fmt;
    idx = a.arguments().indexOf(QLatin1String("-f"));

    if (idx > 0)
        fmt = a.arguments().at(idx + 1);

    QString opt;
    QVariantHash decopt;
    idx = decName.indexOf(QLatin1String(":"));

    if (idx > 0)
    {
        opt     = decName.right(decName.size() - idx -1);
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

    VideoDecoder* dec = VideoDecoder::create(decName.toLatin1().constData());

    if (!dec)
    {
        qCCritical(DIGIKAM_TESTS_LOG)
            << QString::asprintf("Can not find decoder: %s",
                decName.toUtf8().constData());

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
    QElapsedTimer timer;
    timer.start();
    int count                = 0;
    int vstream              = demux.videoStream();
    VideoEncoder* const venc = VideoEncoder::create("FFmpeg");
    venc->setCodecName(cv);

    //venc->setCodecName("png");

    venc->setBitRate(1024 * 1024);

    //venc->setPixelFormat(VideoFormat::Format_RGBA32);

    AVMuxer mux;

    //mux.setMedia("/Users/wangbin/Movies/m3u8/bbb%05d.ts");
    //mux.setMedia("/Users/wangbin/Movies/img2/bbb%05d.png");

    mux.setMedia(outFile);
    QVariantHash muxopt, avfopt;
    avfopt[QString::fromLatin1("segment_time")]      = 4;
    avfopt[QString::fromLatin1("segment_list_size")] = 0;
    avfopt[QString::fromLatin1("segment_list")]      = outFile.left(outFile.lastIndexOf(QLatin1Char('/')) + 1)
                                                       .append(QString::fromLatin1("index.m3u8"));
    avfopt[QString::fromLatin1("segment_format")]    = QString::fromLatin1("mpegts");
    muxopt[QString::fromLatin1("avformat")]          = avfopt;
    qreal fps                                        = 0;

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

            if (!frame)
                continue;

            if (!venc->isOpen())
            {
                venc->setWidth(frame.width());
                venc->setHeight(frame.height());

                if (!venc->open())
                {
                    qCCritical(DIGIKAM_TESTS_LOG) << QString::asprintf("failed to open encoder");

                    return 1;
                }
            }

            if (!mux.isOpen())
            {
                mux.copyProperties(venc);
                mux.setOptions(muxopt);

                if (!fmt.isEmpty())
                    mux.setFormat(fmt);

                //mux.setFormat("segment");
                //mux.setFormat("image2");

                if (!mux.open())
                {
                    qCCritical(DIGIKAM_TESTS_LOG) << QString::asprintf("failed to open muxer");

                    return 1;
                }

                //mux.setOptions(muxopt);
            }

            if (frame.pixelFormat() != venc->pixelFormat())
                frame = frame.to(venc->pixelFormat());

            if (venc->encode(frame))
            {
                Packet pkt2(venc->encoded());
                mux.writeVideo(pkt2);
                count++;

                if ((count % 20) == 0)
                {
                    fps = qreal(count * 1000) / qreal(timer.elapsed());
                }

                qCDebug(DIGIKAM_TESTS_LOG) << "decode count:"
                                           << count
                                           << "fps:"
                                           << fps
                                           << "frame size:"
                                           << frame.width() << "x" << frame.height()
                                           << "==>" << frame.data().size();
            }
        }
    }

    // get delayed frames

    while (venc->encode())
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote() << QString::asprintf("encode delayed frames...\r");
        Packet pkt3(venc->encoded());
        mux.writeVideo(pkt3);
    }

    qint64 elapsed = timer.elapsed();
    int msec       = elapsed / 1000LL + 1;

    qCDebug(DIGIKAM_TESTS_LOG).noquote()
        << QString::asprintf("decoded frames: %d, time: %d, average speed: %d",
            count, msec, count / msec);

    venc->close();
    mux.close();

    return 0;
}
