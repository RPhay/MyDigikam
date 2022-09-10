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

#include "VideoThread.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include "AVThread_p.h"
#include "Packet.h"
#include "AVClock.h"
#include "VideoCapture.h"
#include "VideoDecoder.h"
#include "VideoRenderer.h"
#include "QtAV_Statistics.h"
#include "Filter.h"
#include "FilterContext.h"
#include "OutputSet.h"
#include "AVCompat.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN VideoThreadPrivate : public AVThreadPrivate
{
public:

    VideoThreadPrivate()
        : AVThreadPrivate()
    {
    }

    ~VideoThreadPrivate()
    {
        // not necessary context is managed by filters.

        if (filter_context)
        {
            delete filter_context;
            filter_context = nullptr;
        }
    }

public:

    VideoFrameConverter conv;
    qreal               force_fps       = 0;        ///< <=0: try to use pts. if no pts in stream(guessed by 5 packets), use |force_fps|

    // not const.
    int                 force_dt        = 0;        ///< unit: ms. force_fps = 1/force_dt.

    double              pts             = 0.0;      ///< current decoded pts. for capture. TODO: remove
    VideoCapture*       capture         = nullptr;
    VideoFilterContext* filter_context  = nullptr;  ///< TODO: use own smart ptr. QSharedPointer "=" is ugly
    VideoFrame          displayed_frame;
};

VideoThread::VideoThread(QObject* const parent)
    : AVThread(*new VideoThreadPrivate(), parent)
{
}

// it is called in main thread usually, but is being used in video thread,

VideoCapture* VideoThread::setVideoCapture(VideoCapture* const cap)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("setCapture %p", cap);
    DPTR_D(VideoThread);

    QMutexLocker locker(&d.mutex);
    VideoCapture* const old = d.capture;
    d.capture               = cap;

    if (old)
    {
        disconnect(old, SIGNAL(requested()),
                   this, SLOT(addCaptureTask()));
    }

    if (cap)
    {
        connect(cap, SIGNAL(requested()),
                this, SLOT(addCaptureTask()));
    }

    if (cap && cap->autoSave() && cap->name.isEmpty())
    {
        // statistics is already set by AVPlayerCore

        cap->setCaptureName(QFileInfo(d.statistics->url).completeBaseName());
    }

    return old;
}

VideoCapture* VideoThread::videoCapture() const
{
    return d_func().capture;
}

void VideoThread::addCaptureTask()
{
    if (!isRunning())
        return;

    class Q_DECL_HIDDEN CaptureTask : public QRunnable
    {
    public:

        explicit CaptureTask(VideoThread* const vt)
            : vthread(vt)
        {
        }

        void run()
        {
            VideoCapture* const vc = vthread->videoCapture();

            if (!vc)
                return;

            VideoFrame frame(vthread->displayedFrame());
/*
            vthread->applyFilters(frame);
*/
            vc->setVideoFrame(frame);
            vc->start();
        }

    private:

        VideoThread* vthread = nullptr;

    private:

        Q_DISABLE_COPY(CaptureTask);
    };

    scheduleTask(new CaptureTask(this));
}

void VideoThread::clearRenderers()
{
    d_func().outputSet->sendVideoFrame(VideoFrame());
}

VideoFrame VideoThread::displayedFrame() const
{
    return d_func().displayed_frame;
}

void VideoThread::setFrameRate(qreal value)
{
    DPTR_D(VideoThread);

    d.force_fps = value;

    if (d.force_fps != 0.0)
    {
        d.force_dt = int(1000.0 / d.force_fps);
    }
    else
    {
        d.force_dt = 0;
    }
}

void VideoThread::setBrightness(int val)
{
    setEQ(val, 101, 101);
}

void VideoThread::setContrast(int val)
{
    setEQ(101, val, 101);
}

void VideoThread::setSaturation(int val)
{
    setEQ(101, 101, val);
}

void VideoThread::setEQ(int b, int c, int s)
{
    class Q_DECL_HIDDEN EQTask : public QRunnable
    {
    public:

        explicit EQTask(VideoFrameConverter* const c)
            : brightness(0),
              contrast  (0),
              saturation(0),
              conv      (c)
        {
/*
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("EQTask tid=%p",
                    QThread::currentThread());
*/
        }

        void run()
        {
            conv->setEq(brightness, contrast, saturation);
        }

    public:

        int brightness            = 0;
        int contrast              = 0;
        int saturation            = 0;

    private:

        VideoFrameConverter* conv = nullptr;

    private:

        Q_DISABLE_COPY(EQTask);
    };

    DPTR_D(VideoThread);

    EQTask* const task = new EQTask(&d.conv);
    task->brightness   = b;
    task->contrast     = c;
    task->saturation   = s;

    if (isRunning())
    {
        scheduleTask(task);
    }
    else
    {
        task->run();

        delete task;
    }
}

void VideoThread::applyFilters(VideoFrame& frame)
{
    DPTR_D(VideoThread);

    QMutexLocker locker(&d.mutex);
    Q_UNUSED(locker);

    if (!d.filters.isEmpty())
    {
        // sort filters by format. vo->defaultFormat() is the last

        Q_FOREACH (Filter* const filter, d.filters)
        {
            VideoFilter* const vf = static_cast<VideoFilter*>(filter);

            if (!vf->isEnabled())
                continue;

            if (vf->prepareContext(d.filter_context, d.statistics, &frame))
                vf->apply(d.statistics, &frame);
        }
    }
}

// filters on vo will not change video frame, so it's safe to protect frame only in every individual vo

bool VideoThread::deliverVideoFrame(VideoFrame& frame)
{
    DPTR_D(VideoThread);

    /*
     * TODO: video renderers sorted by preferredPixelFormat() and convert in AVOutputSet.
     * Convert only once for the renderers has the same preferredPixelFormat().
     */

    d.outputSet->lock();
    QList<AVOutput*> outputs = d.outputSet->outputs();
    VideoRenderer* vo        = nullptr;

    if (!outputs.isEmpty())
        vo = static_cast<VideoRenderer*>(outputs.first());

    if (
        vo && (!vo->isSupported(frame.pixelFormat()) ||
        (vo->isPreferredPixelFormatForced() && (vo->preferredPixelFormat() != frame.pixelFormat())))
       )
    {
        VideoFormat fmt(frame.format());

        if (fmt.hasPalette() || fmt.isRGB())
            fmt = VideoFormat::Format_RGB32;
        else
            fmt = vo->preferredPixelFormat();

        VideoFrame outFrame(d.conv.convert(frame, fmt));

        if (!outFrame.isValid())
        {
            d.outputSet->unlock();

            return false;
        }

        frame = outFrame;
    }

    d.outputSet->sendVideoFrame(frame); // TODO: group by format, convert group by group
    d.outputSet->unlock();

    Q_EMIT frameDelivered();

    return true;
}

// TODO: if output is null or dummy, the use duration to wait

void VideoThread::run()
{
    DPTR_D(VideoThread);

    if (!d.dec || !d.dec->isAvailable() || !d.outputSet)
        return;
/*
    resetState(); // we can't reset the thread state from here
*/
    if (d.capture->autoSave())
    {
        d.capture->setCaptureName(QFileInfo(d.statistics->url).completeBaseName());
    }

    // not necessary context is managed by filters.

    d.filter_context           = VideoFilterContext::create(VideoFilterContext::QtPainter);
    VideoDecoder* dec          = static_cast<VideoDecoder*>(d.dec);
    Packet pkt;
    QVariantHash* dec_opt      = &d.dec_opt_normal; // TODO: restore old framedrop option after seek

    /*!
     * if we skip some frames(e.g. seek, drop frames to speed up), then first frame to decode must
     * be a key frame for hardware decoding. otherwise may crash
     */

    bool wait_key_frame        = false;
    int nb_dec_slow            = 0;
    int nb_dec_fast            = 0;
    qint32 seek_count          = 0; // wm4 says: 1st seek can not use frame drop for decoder

    // TODO: kNbSlowSkip depends on video fps, ensure slow time <= 2s

    /*
     * kNbSlowSkip: if video frame slow count >= kNbSlowSkip, skip decoding all frames until next keyframe reaches.
     * if slow count > kNbSlowSkip/2, skip rendering every 3 or 6 frames
     */

    const int kNbSlowSkip      = 20;

    // kNbSlowFrameDrop: if video frame slow count > kNbSlowFrameDrop, skip decoding nonref frames.
    // only some of ffmpeg based decoders support it.

    const int kNbSlowFrameDrop = 10;
    bool sync_audio            = (d.clock->clockType() == AVClock::AudioClock);
    bool sync_video            = (d.clock->clockType() == AVClock::VideoClock); // no frame drop
    const qint64 start_time    = QDateTime::currentMSecsSinceEpoch();
    qreal v_a                  = 0;
    int nb_no_pts              = 0;

    Q_UNUSED(sync_audio);
    Q_UNUSED(sync_video);

    // bool wait_audio_drain

    const char* pkt_data       = nullptr; // workaround for libav9 decode fail but error code >= 0
    qint64 last_deliver_time   = 0;
    int sync_id                = 0;

    while (!d.stop)
    {
        processNextTask();

        // TODO: why put it at the end of loop then stepForward() not work?
        // processNextTask tryPause(timeout) and  and continue outter loop

        if (d.render_pts0 < 0)
        {
            // no pause when seeking

            if (tryPause())
            {
                // DO NOT continue, or stepForward() will fail
            }
            else
            {
                if (isPaused())
                    continue; // timeout. process pending tasks
            }
        }

        if (d.seek_requested)
        {
            d.seek_requested = false;

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("request seek video thread");

            // last decode failed and pkt is valid, reset pkt to force take the next packet if seek is requested

            pkt              = Packet();
            msleep(1);
        }
        else
        {
            // d.render_pts0 < 0 means seek finished here

            if (d.clock->syncId() > 0)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("video thread wait to sync end for sync id: %d",
                        d.clock->syncId());

                if ((d.render_pts0 < 0) && (sync_id > 0))
                {
                    msleep(10);
                    v_a = 0;

                    continue;
                }
            }
            else
            {
                sync_id = 0;
            }
        }

        if (!pkt.isValid() && !pkt.isEOF())
        {
            // can't seek back if eof packet is read

            pkt = d.packets.take(); // wait to dequeue

            // TODO: push pts history here and reorder
        }

        if (pkt.isEOF())
        {
            wait_key_frame = false;

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("video thread gets an eof packet.");
        }
        else
        {
/*
            qCDebug(DIGIKAM_QTAV_LOG) << pkt.position << " pts:" <<pkt.pts;
*/
            // Compare to the clock

            if (!pkt.isValid())
            {
                // may be we should check other information. invalid packet can come from

                wait_key_frame = true;

                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("Invalid packet! flush video codec context!!!!!!!!!! video packet queue size: %d",
                        d.packets.size());

                // d.dec instead of dec because d.dec maybe changed in processNextTask() but dec is not

                d.dec->flush();

                d.render_pts0 = pkt.pts;
                sync_id       = pkt.position;

                if (pkt.pts >= 0)
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("video seek: %.3f, id: %d",
                            d.render_pts0, sync_id);
                }

                d.pts_history = ring<qreal>(d.pts_history.capacity());
                v_a           = 0;

                continue;
            }
        }

        if ((pkt.pts <= 0) && !pkt.isEOF() && (pkt.data.size() > 0))
        {
            nb_no_pts++;
        }
        else
        {
            nb_no_pts = 0;
        }

        if (nb_no_pts > 5)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("the stream may have no pts. force fps to: %f/%f",
                    (d.force_fps < 0) ? -d.force_fps : 24, d.force_fps);

            d.clock->setClockAuto(false);
            d.clock->setClockType(AVClock::VideoClock);

            if      (d.force_fps < 0)
                setFrameRate(-d.force_fps);
            else if (d.force_fps == 0)
                setFrameRate(24);
        }

        if      (d.clock->clockType() == AVClock::AudioClock)
        {
            sync_audio = true;
            sync_video = false;
        }
        else if (d.clock->clockType() == AVClock::VideoClock)
        {
            sync_audio = false;
            sync_video = true;
        }
        else
        {
            sync_audio = false;
            sync_video = false;
        }

        const qreal dts = pkt.dts; // FIXME: pts and dts

        // TODO: delta ref time
        // if dts is invalid, diff can be very small (<0) and video will be decoded
        // and rendered(display_wait is disabled for now) immediately

        qreal diff = ((dts > 0) ? dts - d.clock->value() + v_a : v_a);

        if (pkt.isEOF())
            diff = qMin<qreal>(1.0, qMax<qreal>(d.delay, 1.0 / d.statistics->video_only.currentDisplayFPS()));

        if ((diff < 0) && sync_video)
            diff = 0; // this ensures no frame drop

        if (diff > kSyncThreshold)
        {
            nb_dec_fast++;
        }
        else
        {
            nb_dec_fast /= 2;
        }

        bool seeking = (d.render_pts0 >= 0.0);

        if (seeking)
        {
            nb_dec_slow = 0;
            nb_dec_fast = 0;
        }
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("nb_fast/slow: %d/%d. diff: %f, delay: %f, dts: %f, clock: %f",
                nb_dec_fast, nb_dec_slow, diff, d.delay, dts, clock()->value());
*/
        if ((d.delay < -0.5) && (d.delay > diff))
        {
            if (!seeking)
            {
                // ensure video will not later than 2s

                if ((diff < -2) || ((nb_dec_slow > kNbSlowSkip) && (diff < -1.0) && !pkt.hasKeyFrame))
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("video is too slow. skip decoding until next key frame.");

                    // TODO: when to reset so frame drop flag can reset?

                    nb_dec_slow    = 0;
                    wait_key_frame = true;
                    pkt            = Packet();
                    v_a            = 0;

                    // TODO: use discard flag

                    continue;
                }
                else
                {
                    nb_dec_slow++;

                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("frame slow count: %d. v-a: %.3f",
                            nb_dec_slow, diff);
                }
            }
        }
        else
        {
            if (nb_dec_slow >= kNbSlowFrameDrop)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("decrease 1 slow frame: %d",
                        nb_dec_slow);

                // nb_dec_slow < kNbSlowFrameDrop will reset decoder frame drop flag

                nb_dec_slow = qMax(0, nb_dec_slow - 1);
            }
        }

        // can not change d.delay after! we need it to comapre to next loop

        d.delay = diff;

        /*
         *after seeking forward, a packet may be the old, v packet may be
         *the new packet, then the d.delay is very large, omit it.
         */

        if (seeking)
            diff = 0; // TODO: here?

        if (!sync_audio && (diff > 0))
        {
            // wait to dts reaches
            // d.force_fps>0: wait after decoded before deliver

            if (d.force_fps <= 0) // || !qFuzzyCompare(d.clock->speed(), 1.0))
            {
                // TODO: count decoding and filter time, or decode immediately but wait for display

                waitAndCheck(diff * 1000UL, dts); 
            }

            diff = 0; // TODO: can not change delay!
        }

        // update here after wait. TODO: use decoded timestamp/guessed next pts?

        d.clock->updateVideoTime(dts); // FIXME: dts or pts?
        bool skip_render = false;

        if      (qAbs(diff) < 0.5)
        {
            if (diff < -kSyncThreshold)
            {
                // Speed up. drop frame?

                //continue;
            }
        }
        else if (!seeking)
        {
            // when to drop off?

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("delay %fs @%.3fs pts:%.3f",
                    diff, d.clock->value(), pkt.pts);

            if (diff < 0)
            {
                if (nb_dec_slow > kNbSlowSkip)
                {
                    skip_render = !pkt.hasKeyFrame && (nb_dec_slow % 2);
                }
            }
            else
            {
                const double s = qMin<qreal>(0.01 * (nb_dec_fast >> 1), diff);

                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("video too fast!!! sleep %.2f s, nb fast: %d, v_a: %.4f",
                        s, nb_dec_fast, v_a);

                waitAndCheck(s * 1000UL, dts);

                diff           = 0;
                skip_render    = false;
            }
        }

        // audio packet not cleaned up?

        if ((diff > 0) && (diff < 1.0) && !seeking)
        {
            // can not change d.delay here! we need it to comapre to next loop

            waitAndCheck(diff * 1000UL, dts);
        }

        if (wait_key_frame)
        {
            if (!pkt.hasKeyFrame)
            {
                qCDebug(DIGIKAM_QTAV_LOG) << "waiting for key frame. queue size:"
                                          << d.packets.size() << "pkt.size:"
                                          << pkt.data.size();

                pkt = Packet();
                v_a = 0;

                continue;
            }

            wait_key_frame = false;
        }

        QVariantHash* const dec_opt_old = dec_opt;

        if (!seeking || ((pkt.pts - d.render_pts0) >= -0.05))
        {
            // MAYBE not seeking. We should not drop the frames near the seek target.
            // FIXME: use packet pts distance instead of -0.05 (20fps)

            if (seeking)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("seeking... pkt.pts - d.render_pts0: %.3f",
                        pkt.pts - d.render_pts0);
            }

            if (nb_dec_slow < kNbSlowFrameDrop)
            {
                if (dec_opt == &d.dec_opt_framedrop)
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("frame drop=>normal. nb_dec_slow: %d",
                            nb_dec_slow);

                    dec_opt = &d.dec_opt_normal;
                }
            }
            else
            {
                if (dec_opt == &d.dec_opt_normal)
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("frame drop=>noref. nb_dec_slow: %d too slow",
                            nb_dec_slow);

                    dec_opt = &d.dec_opt_framedrop;
                }
            }
        }
        else
        {
            // seeking

            if ((seek_count > 0) && d.drop_frame_seek)
            {
                if (dec_opt == &d.dec_opt_normal)
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote() <<
                        QString::asprintf("seeking... pkt.pts - d.render_pts0: %.3f, frame drop=>noref. nb_dec_slow: %d",
                            pkt.pts - d.render_pts0, nb_dec_slow);

                    dec_opt = &d.dec_opt_framedrop;
                }
            }
            else
            {
                seek_count = -1;
            }
        }

        // decoder maybe changed in processNextTask(). code above MUST use d.dec but not dec

        if (dec != static_cast<VideoDecoder*>(d.dec))
        {
            dec = static_cast<VideoDecoder*>(d.dec);

            if (!pkt.hasKeyFrame)
            {
                wait_key_frame = true;
                v_a            = 0;

                continue;
            }

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("decoder changed. decoding key frame");
        }

        if (dec_opt != dec_opt_old)
            dec->setOptions(*dec_opt);

        if (!dec->decode(pkt))
        {
            d.pts_history.push_back(d.pts_history.back());
/*
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Decode video failed. undecoded: %d/%d",
                    dec->undecodedSize(), pkt.data.size());
*/
            if (pkt.isEOF())
            {
                Q_EMIT eofDecoded();

                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("video decode eof done. d.render_pts0: %.3f",
                        d.render_pts0);

                if (d.render_pts0 >= 0)
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("video seek done at eof pts: %.3f. id: %d",
                            d.pts_history.back(), sync_id);

                    d.render_pts0 = -1;
                    d.clock->syncEndOnce(sync_id);

                    Q_EMIT seekFinished(qint64(d.pts_history.back() * 1000.0));

                    if      (seek_count == -1)
                        seek_count = 1;
                    else if (seek_count > 0)
                        seek_count++;
                }

                if (!pkt.position)
                    break;
            }

            pkt = Packet();

            continue;
        }

        // reduce here to ensure to decode the rest data in the next loop

        if (!pkt.isEOF())
            pkt.skip(pkt.data.size() - dec->undecodedSize());

        VideoFrame frame = dec->frame();

        if (!frame.isValid())
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN) << "invalid video frame from decoder. undecoded data size:"
                                             << pkt.data.size();

            if (pkt_data == pkt.data.constData()) // FIXME: for libav9. what about other versions?
                pkt = Packet();
            else
                pkt_data = pkt.data.constData();

            continue;
        }

        pkt_data = pkt.data.constData();

        if (frame.timestamp() < 0)
            frame.setTimestamp(pkt.pts);          // pkt.pts is wrong. >= real timestamp

        const qreal pts = frame.timestamp();
        d.pts_history.push_back(pts);

        // seek finished because we can ensure no packet before seek decoded when render_pts0 is set
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("pts0: %f, pts: %f, clock: %d",
                d.render_pts0, pts, d.clock->clockType());
*/
        if (d.render_pts0 >= 0.0)
        {
            if (pts < d.render_pts0)
            {
                if (!pkt.isEOF())
                    pkt = Packet();

                v_a = 0;

                continue;
            }

            d.render_pts0 = -1;

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("video seek finished @%f. id: %d",
                    pts, sync_id);

            d.clock->syncEndOnce(sync_id);

            Q_EMIT seekFinished(qint64(pts * 1000.0));

            if      (seek_count == -1)
                seek_count = 1;
            else if (seek_count > 0)
                seek_count++;
        }

        if (skip_render)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("skip rendering @%.3f", pts);

            pkt = Packet();
            v_a = 0;

            continue;
        }

        Q_ASSERT(d.statistics);

        d.statistics->video.current_time = QTime(0, 0, 0).addMSecs(int(pts * 1000.0)); // TODO: is it expensive?
        applyFilters(frame);

        // while can pause, processNextTask, not call outset.puase which is deperecated

        while (d.outputSet->canPauseThread())
        {
            d.outputSet->pauseThread(100);
/*
            tryPause(100);
*/
            processNextTask();
        }
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("force fps: %f dt: %d",
                d.force_fps, d.force_dt);
*/
        if      (d.force_dt > 0)
        {
            // && qFuzzyCompare(d.clock->speed(), 1.0)) {

            const qint64 now   = QDateTime::currentMSecsSinceEpoch();
            const qint64 delta = qint64(d.force_dt) - (now - last_deliver_time);

            if (frame.timestamp() <= 0)
            {
                // TODO: what if seek happens during playback?

                const int msecs_started(now + qMax(0LL, delta) - start_time);
                frame.setTimestamp(qreal(msecs_started) / 1000.0);
                clock()->updateValue(frame.timestamp()); // external clock?
            }

            if (delta > 0LL)
            {
                // limit up bound?

                waitAndCheck((ulong)delta, -1); // wait and not compare pts-clock
            }
        }
        else if (false)
        {
            // FIXME: may block a while when seeking

            const qreal display_wait = pts - clock()->value();

            if (!seeking && (display_wait > 0.0))
            {
                // wait to pts reaches. TODO: count rendering time
/*
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("wait %f to display for pts %f-%f",
                        display_wait, pts, clock()->value());
*/
                if (display_wait < 1.0)
                    waitAndCheck(display_wait * 1000UL, pts); // TODO: count decoding and filter time
            }
        }

        // no return even if d.stop is true. ensure frame is displayed.
        // otherwise playing an image may be failed to display

        if (!deliverVideoFrame(frame))
            continue;
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("clock.diff: %.3f", d.clock->diff());
*/
        if (d.force_dt > 0)
            last_deliver_time = QDateTime::currentMSecsSinceEpoch();

        // TODO: store original frame. now the frame is filtered and maybe converted to renderer perferred format

        d.displayed_frame = frame;

        if (d.clock->clockType() == AVClock::AudioClock)
        {
            const qreal v_a_ = frame.timestamp() - d.clock->value();

            if (!qFuzzyIsNull(v_a_))
            {
                if      (v_a_ < -0.1)
                {
                    if (v_a <= v_a_)
                        v_a += -0.01;
                    else
                        v_a = (v_a_ + v_a) * 0.5;
                }
                else if (v_a_ < -0.002)
                {
                    v_a += -0.001;
                }
                else if (v_a_ < 0.002)
                {
                }
                else if (v_a_ < 0.1)
                {
                    v_a += 0.001;
                }
                else
                {
                    if (v_a >= v_a_)
                        v_a += 0.01;
                    else
                        v_a = (v_a_ + v_a) * 0.5;
                }

                if ((v_a < -2) || (v_a > 2))
                   v_a /= 2.0;
            }
/*
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("v_a:%.4f, v_a_: %.4f",
                    v_a, v_a_);
*/
        }
    }

#if 0

    if (d.stop)
    {
        // user stop

        // decode eof?

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("decoding eof...");

        while (d.dec && d.dec->decode(Packet::createEOF()))
        {
            d.dec->flush();
        }
    }

#endif

    d.packets.clear();

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("Video thread stops running...");
}

} // namespace QtAV
