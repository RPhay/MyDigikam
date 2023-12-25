/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "vidslidetask.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QImage>
#include <QSize>
#include <QPainter>
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// QtAv includes

#include "VideoFrame.h"
#include "VideoEncoder.h"
#include "AVMuxer.h"

// Local includes

#include "frameutils.h"
#include "dfileoperations.h"
#include "transitionmngr.h"
#include "effectmngr.h"
#include "digikam_debug.h"
#include "digikam_config.h"

using namespace QtAV;

namespace Digikam
{

class Q_DECL_HIDDEN VidSlideTask::Private
{
public:

    Private() = default;

    bool encodeFrame(VideoFrame& vframe,
                     VideoEncoder* const venc,
                     AVMuxer& mux);

public:

    VidSlideSettings* settings = nullptr;
};

bool VidSlideTask::Private::encodeFrame(VideoFrame& vframe,
                                        VideoEncoder* const venc,
                                        AVMuxer& mux)
{
    Packet vpkt;

    if (vframe.pixelFormat() != venc->pixelFormat())
    {
        vframe = vframe.to(venc->pixelFormat());
    }

    if (venc->encode(vframe))
    {
        vpkt = venc->encoded();

        if (vpkt.isValid())
        {
            mux.writeVideo(vpkt);
        }

        return true;
    }

    return false;
}

// -------------------------------------------------------

VidSlideTask::VidSlideTask(VidSlideSettings* const settings)
    : ActionJob(),
      d        (new Private)
{
    d->settings = settings;
}

VidSlideTask::~VidSlideTask()
{
    cancel();
    delete d;
}

void VidSlideTask::run()
{
    // ---------------------------------------------
    // Setup output video file

    QUrl dest       = d->settings->outputDir;
    dest            = dest.adjusted(QUrl::StripTrailingSlash);
    dest.setPath(dest.path() + QLatin1String("/videoslideshow.") + d->settings->videoFormat());
    QString outFile = dest.toLocalFile();
    QFileInfo fi(outFile);

    if (fi.exists() && (d->settings->conflictRule != FileSaveConflictBox::OVERWRITE))
    {
        outFile = DFileOperations::getUniqueFileUrl(dest).toLocalFile();
    }

    // ---------------------------------------------
    // Setup Video Encoder

    VideoEncoder* const venc = VideoEncoder::create("FFmpeg");
    venc->setCodecName(d->settings->videoCodec());
    venc->setBitRate(d->settings->videoBitRate());
    venc->setFrameRate(d->settings->videoFrameRate());

    QSize osize              = d->settings->videoSize();
    venc->setWidth(osize.width());
    venc->setHeight(osize.height());

    if (!venc->open())
    {
        Q_EMIT signalMessage(i18n("Failed to open video encoder"), true);
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to open video encoder";
        Q_EMIT signalDone(false);

        return;
    }

    // ---------------------------------------------
    // Setup Muxer

    AVMuxer mux;
    mux.setMedia(outFile);
    mux.copyProperties(venc);  // Setup video encoder
/*
    // Segments muxer ffmpeg options. See : https://www.ffmpeg.org/ffmpeg-formats.html#Options-11

    QVariantHash avfopt;
    avfopt[QLatin1String("segment_time")]      = 4;
    avfopt[QLatin1String("segment_list_size")] = 0;
    avfopt[QLatin1String("segment_format")]    = QLatin1String("mpegts");
    avfopt[QLatin1String("segment_list")]      = outFile.left(outFile.lastIndexOf(QLatin1Char('/'))+1)
                                                              .append(QLatin1String("index.m3u8"));
    QVariantHash muxopt;
    muxopt[QLatin1String("avformat")]          = avfopt;

    mux.setOptions(muxopt);
*/
    if (!mux.open())
    {
        Q_EMIT signalMessage(i18n("Failed to open muxer"), true);
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to open muxer";
        Q_EMIT signalDone(false);

        return;
    }

    QImage qiimg;

    // ---------------------------------------------
    // Loop to encode frames with images list

    TransitionMngr transmngr;
    transmngr.setOutputSize(osize);

    EffectMngr effmngr;
    effmngr.setOutputSize(osize);
    effmngr.setFrames(d->settings->imgFrames);

    for (int i = 0 ; ((i < d->settings->inputImages.count() + 1) && !m_cancel) ; ++i)
    {
        if (i == 0)
        {
            qiimg = FrameUtils::makeFramedImage(QString(), osize);
        }

        QString ofile;

        if (i < d->settings->inputImages.count())
        {
            ofile = d->settings->inputImages[i].toLocalFile();
        }

        QImage qoimg = FrameUtils::makeFramedImage(ofile, osize);

        // -- Transition encoding ----------

        transmngr.setInImage(qiimg);
        transmngr.setOutImage(qoimg);
        transmngr.setTransition(d->settings->transition);

        int ttmout = 0;

        do
        {
            VideoFrame frame(transmngr.currentFrame(ttmout));

            if (!d->encodeFrame(frame, venc, mux))
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot encode transition frame";
            }
        }
        while ((ttmout != -1) && !m_cancel);

        // -- Images encoding ----------

        if (i < d->settings->inputImages.count())
        {
            VideoFrame frame;
            int count  = 0;
            int itmout = 0;
            effmngr.setImage(qoimg);
            effmngr.setEffect(d->settings->vEffect);

            do
            {
                qiimg = effmngr.currentFrame(itmout);
                frame = VideoFrame(qiimg);

                if (d->encodeFrame(frame, venc, mux))
                {

                    ++count;
/*
                    qCDebug(DIGIKAM_GENERAL_LOG) << ofile
                                                 << " => encode count:" << count
                                                 << "frame size:"       << frame.width()
                                                 << "x"                 << frame.height();
*/
                }
            }
            while ((count < d->settings->imgFrames) && !m_cancel);
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "Encoded image" << i << "done";

        Q_EMIT signalMessage(i18n("Encoding %1 Done", ofile), false);
        Q_EMIT signalProgress(i);
    }

    // ---------------------------------------------
    // Get delayed frames

    qCDebug(DIGIKAM_GENERAL_LOG) << "Encode delayed frames...";

    while (venc->encode() && !m_cancel)
    {
        Packet vpkt(venc->encoded());

        if (vpkt.isValid())
        {
            mux.writeVideo(vpkt);
        }
    }

    // ---------------------------------------------
    // Cleanup

    venc->close();
    mux.close();

    if (!m_cancel)
    {
        Q_EMIT signalMessage(i18n("Output video is %1", outFile), false);
        d->settings->outputVideo = outFile;
    }

    Q_EMIT signalDone(!m_cancel);
}

} // namespace Digikam

#include "moc_vidslidetask.cpp"
