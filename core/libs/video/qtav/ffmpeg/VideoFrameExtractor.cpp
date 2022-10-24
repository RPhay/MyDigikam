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

#include "VideoFrameExtractor.h"

// Qt includes

#include <QCoreApplication>
#include <QQueue>
#include <QRunnable>
#include <QScopedPointer>
#include <QStringList>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

// Local includes

#include "VideoCapture.h"
#include "VideoDecoder.h"
#include "AVDemuxer.h"
#include "Packet.h"
#include "QtAV_BlockingQueue.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN ExtractThread : public QThread
{
    Q_OBJECT

public:

    ExtractThread(QObject* const parent = nullptr)
        : QThread   (parent),
          timeout_ms(50UL),
          stop      (false)
    {
        // avoid too frequent -- we only care about the latest request
        // whether it be for a frame or to stop thread or whatever else.

        tasks.setCapacity(1);
    }

    ~ExtractThread()
    {
        waitStop();
    }

    void waitStop()
    {
        if (!isRunning())
            return;

        scheduleStop();
        wait();
    }

    unsigned long timeout_ms;

    void addTask(QRunnable* t)
    {
        // Note that while a simpler solution would have been to not use
        // a custom 'Task' mechanism but rather to use signals
        // or QEvent posting to this thread -- the approach here has a very
        // advantageous property. Namely, duplicate repeated requests for
        // a frame end up getting dropped and only the latest request gets
        // serviced.  This interoperates well with eg VideoPreviewWidget
        // which really only cares about the latest frame requested by the user.

        while ((tasks.size() >= tasks.capacity()) && (tasks.capacity() > 0))
        {
            // Race condition existed here -- to avoid it we need to take()
            // with a timeout (to make sure it doesn't hang) and check return value.
            // This is because extractor thread is also calling .take() at the same time,
            // and in rare cases the above expression in the while loop evaluates to true
            // while by the time the below line executes the underlying queue may become empty.
            // This led to very occasional hangs.  Hence this .take() call was modified to include
            // a timeout.

            QRunnable* const task = tasks.take(timeout_ms); // clear for seek & stop task

            if (task && task->autoDelete())
                delete task;
        }

        if (!tasks.put(t,timeout_ms))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("ExtractThread::addTask -- trying to add a task to an already-full queue!");
        }
    }

    void scheduleStop()
    {
        class Q_DECL_HIDDEN StopTask : public QRunnable
        {
        public:

            explicit StopTask(ExtractThread* const t)
                : thread(t)
            {
            }

            void run()
            {
                thread->stop = true;
            }

        private:

            ExtractThread* thread = nullptr;

        private:

            Q_DISABLE_COPY(StopTask);
        };

        addTask(new StopTask(this));
    }

protected:

    virtual void run()
    {
        while (!stop)
        {
            QRunnable* const task = tasks.take();

            if (task)
            {
                task->run();

                if (task->autoDelete())
                    delete task;
            }
        }

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("ExtractThread exiting...");
    }

public:

    volatile bool stop;

private:

    BlockingQueue<QRunnable*> tasks;
};

// FIXME: avcodec_close() crash

const int kDefaultPrecision = 500;

class Q_DECL_HIDDEN VideoFrameExtractorPrivate : public DPtrPrivate<VideoFrameExtractor>
{
public:

    VideoFrameExtractorPrivate()
        : abort_seek    (false),
          async         (true),
          has_video     (true),
          loading       (false),
          auto_extract  (true),
          auto_precision(true),
          seek_count    (0),
          position      (-2 * kDefaultPrecision),
          precision     (kDefaultPrecision),
          decoder       (nullptr)
    {
        QVariantHash opt;
        opt[QString::fromLatin1("skip_frame")]            = 8;      // 8 for "avcodec", "NoRef" for "FFmpeg". see AVDiscard
        opt[QString::fromLatin1("skip_loop_filter")]      = 8;      // skip all?

        // skip_dict is slower

        dec_opt_framedrop[QString::fromLatin1("avcodec")] = opt;
        opt[QString::fromLatin1("skip_frame")]            = 0;     // 0 for "avcodec", "Default" for "FFmpeg". see AVDiscard
        opt[QString::fromLatin1("skip_loop_filter")]      = 0;
        dec_opt_normal[QString::fromLatin1("avcodec")]    = opt;   // avcodec need correct string or value in libavcodec
        codecs

#if QTAV_HAVE(DXVA)

            // << QLatin1String("DXVA")

#endif // QTAV_HAVE(DXVA)

#if QTAV_HAVE(VAAPI)

            // << QLatin1String("VAAPI")

#endif // QTAV_HAVE(VAAPI)

#if QTAV_HAVE(VDA)

            // << QLatin1String("VDA") // only 1 app can use VDA at a given time

#endif // QTAV_HAVE(VDA)

#if QTAV_HAVE(VIDEOTOOLBOX)

            // << QLatin1String("VideoToolbox")

#endif // QTAV_HAVE(VIDEOTOOLBOX)

            << QLatin1String("FFmpeg");
    }

    ~VideoFrameExtractorPrivate()
    {
        // stop first before demuxer and decoder close to avoid running new seek task after demuxer is closed.

        thread.waitStop();
        releaseResourceInternal();
    }

    bool checkAndOpen()
    {
        const bool loaded = ((demuxer.fileName() == source) && demuxer.isLoaded());

        // we may seek back later when eof got. TODO: remove demuxer.atEnd()

        if (loaded && decoder) // && !demuxer.atEnd())
            return true;

        seek_count = 0;
        decoder.reset(nullptr);

        if (!loaded || demuxer.atEnd())
        {
            demuxer.unload();
            demuxer.setMedia(source);

            if (!demuxer.load())
            {
                return false;
            }
        }

        has_video = (demuxer.videoStreams().size() > 0);

        if (!has_video)
        {
            demuxer.unload();

            return false;
        }

        if (codecs.isEmpty())
            return false;

        if (auto_precision)
        {
            if (demuxer.duration() < 10 * 1000)
                precision = kDefaultPrecision / 2;
            else
                precision = kDefaultPrecision;
        }

        demuxer.setStreamIndex(AVDemuxer::VideoStream, 0);

        Q_FOREACH (const QString& c, codecs)
        {
            VideoDecoder* const vd = VideoDecoder::create(c.toUtf8().constData());

            if (!vd)
                continue;

            decoder.reset(vd);

#ifndef HAVE_FFMPEG_VERSION5

            AVCodecContext* const cctx    = demuxer.videoCodecContext();

#else // ffmpeg >= 5


            AVCodecParameters* const cctx = demuxer.videoCodecContext();

#endif

            if (cctx)
                decoder->setCodecContext(demuxer.videoCodecContext());

            if (!cctx || !decoder->open())
            {
                decoder.reset(nullptr);

                continue;
            }

            QVariantHash opt, va;

            va[QString::fromLatin1("display")] = QString::fromLatin1("X11"); // to support swscale
            opt[QString::fromLatin1("vaapi")]  = va;
            decoder->setOptions(opt);

            break;
        }

        return !!decoder;
    }

    // return the key frame position

    bool extractInPrecision(qint64 value, int range, QString& err, bool& aborted)
    {
        abort_seek = false;
        err        = QString();
        aborted    = false;
        frame      = VideoFrame();

        if (value < demuxer.startTime())
            value += demuxer.startTime();

        demuxer.seek(value);
        const int vstream      = demuxer.videoStream();
        Packet pkt;
        qint64 pts0            = -1;
        bool warn_bad_seek     = true;
        bool warn_out_of_range = true;

        while (!demuxer.atEnd())
        {
            if (abort_seek)
            {
                err     = QLatin1String("abort seek before read");

                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("VideoFrameExtractor abort seek before read");

                aborted = true;

                return false;
            }

            if (!demuxer.readPacket())
                continue;

            if (demuxer.stream() != vstream)
                continue;

            pkt = demuxer.packet();

            if (pts0 < 0LL)
                pts0 = (qint64)(pkt.pts * 1000.0);

            if (((qint64)(pkt.pts * 1000.0) - value) > (qint64)range)
            {
                if (warn_out_of_range)
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("read packet out of range");
                }

                warn_out_of_range = false;

                // No return because decoder needs more packets before the desired frame is decoded

                //return false;
            }
/*
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("video packet: %f", pkt.pts);
*/
            // TODO: always key frame?

            if (pkt.hasKeyFrame)
                break;

            if (warn_bad_seek)
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("Not seek to key frame!!!");
            }

            warn_bad_seek = false;
        }

        // enlarge range if seek to key-frame failed

        const qint64 key_pts     = (qint64)(pkt.pts * 1000.0);
        const bool enlarge_range = ((pts0 >= 0LL) && ((key_pts - pts0) > 0LL));

        if (enlarge_range)
        {
            range = qMax<qint64>(key_pts - value, range);
            qCDebug(DIGIKAM_QTAV_LOG) << "enlarge range => " << range;
        }

        if (!pkt.isValid())
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN)
                << "VideoFrameExtractor failed to get a packet at" << value;

            err = QString().asprintf("failed to get a packet at %lld", value);

            return false;
        }

        // must flush otherwise old frames will be decoded at the beginning

        decoder->flush();

        decoder->setOptions(dec_opt_normal);

        // must decode key frame

        int k = 0;

        while ((k < 2) && !frame.isValid())
        {
            if (abort_seek)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("VideoFrameExtractor abort seek before decoding key frames");

                err     = QLatin1String("abort seek before decoding key frames");
                aborted = true;

                return false;
            }
/*
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("invalid key frame! undecoded: %d",
                    decoder->undecodedSize());
*/
            if (decoder->decode(pkt))
            {
                frame = decoder->frame();
            }

            ++k;
        }

        // if seek backward correctly to key frame, diff0 = t - value <= 0
        // but sometimes seek to no-key frame(and range is enlarged), diff0 >= 0
        // decode key frame

        const int diff0 = qint64(frame.timestamp() * 1000.0) - value;

        if (qAbs(diff0) <= range)
        {
            // TODO: flag forward: result pts must >= value

            if (frame.isValid())
            {
                qCDebug(DIGIKAM_QTAV_LOG)
                    << "VideoFrameExtractor: key frame found @"
                    << frame.timestamp() <<" diff="
                    << diff0 << ". format: " <<  frame.format();

                return true;
            }
        }

        QVariantHash* dec_opt = &dec_opt_normal; // 0: default, 1: framedrop

        // decode at the given position

        while (!demuxer.atEnd())
        {
            if (abort_seek)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("VideoFrameExtractor abort seek after key frame before read");

                err     = QLatin1String("abort seek after key frame before read");
                aborted = true;

                return false;
            }

            if (!demuxer.readPacket())
                continue;

            if (demuxer.stream() != vstream)
                continue;

            pkt           = demuxer.packet();
            const qreal t = pkt.pts;
/*
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("video packet: %f, delta=%lld",
                    t, value - qint64(t*1000.0));
*/
            if (!pkt.isValid())
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("invalid packet. no decode");

                continue;
            }

            if (pkt.hasKeyFrame)
            {
                // FIXME:
/*
                qCCritical(DIGIKAM_QTAV_LOG_CRITICAL)
                    << QString::asprintf("Internal error. Can not be a key frame!");
*/
                //return false; // ??
            }

            qint64 diff                     = qint64(t * 1000.0) - value;
            QVariantHash* const dec_opt_old = dec_opt;

            if ((seek_count == 0) || (diff >= 0))
                dec_opt = &dec_opt_normal;
            else
                dec_opt = &dec_opt_framedrop;

            if (dec_opt != dec_opt_old)
                decoder->setOptions(*dec_opt);

            // invalid packet?

            if (!decoder->decode(pkt))
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("decode failed!");

                frame = VideoFrame();
                err   = QLatin1String("decode failed");

                return false;
            }

            // store the last decoded frame because next frame may be out of range

            const VideoFrame f = decoder->frame();

            if (!f.isValid())
            {
/*
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("VideoFrameExtractor: invalid frame!");
*/
                continue;
            }

            frame               = f;
            const qreal pts     = frame.timestamp();
            const qint64 pts_ms = pts * 1000.0;

            if (pts_ms < value)
                continue;

            diff = pts_ms - value;

            if (qAbs(diff) <= (qint64)range)
            {
                qCDebug(DIGIKAM_QTAV_LOG) << "got frame at" << pts
                                          << "s, diff=" << diff;

                break;
            }

            // if decoder was not flushed, we may get old frame which is acceptable

            if ((diff > range) && (t > pts))
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN) << "out pts out of range. diff="
                                                 << diff << ", range=" << range;

                frame = VideoFrame();
                err   = QString().asprintf("out pts out of range. diff=%lld, range=%d", diff, range);

                return false;
            }
        }

        ++seek_count;

        // now we get the final frame

        if (demuxer.atEnd())
            releaseResourceInternal(false);

        return true;
    }

    void releaseResourceInternal(bool releaseFrame = true)
    {
        if (releaseFrame)
            frame = VideoFrame();

        seek_count = 0;

        // close codec context first.

        decoder.reset(nullptr);
        demuxer.unload();
    }

    void safeReleaseResource()
    {
        class Q_DECL_HIDDEN Cleaner : public QRunnable
        {
        public:

            explicit Cleaner(VideoFrameExtractorPrivate* const pri)
                : p(pri)
            {
            }

            void run()
            {
                p->releaseResourceInternal();
            }

        private:

            VideoFrameExtractorPrivate* p = nullptr;

        private:

            Q_DISABLE_COPY(Cleaner);
        };

        thread.addTask(new Cleaner(this));
    }

    volatile bool                abort_seek;
    bool                         async;
    bool                         has_video;
    bool                         loading;
    bool                         auto_extract;
    bool                         auto_precision;
    int                          seek_count;
    qint64                       position;    ///< only read/written by this->thread(), never read by extractor thread so no lock necessary
    volatile int                 precision;   ///< is volatile because may be written by this->thread() and read by extractor thread but typical use is to not modify it while extract is running
    QString                      source;      ///< is written-to by this->thread() but may be read by extractor thread; important: apparently two threads accessing QString is supported by Qt according to wang-bin, so we aren't guarding this with a lock
    AVDemuxer                    demuxer;
    QScopedPointer<VideoDecoder> decoder;
    VideoFrame                   frame;       ///< important: we only allow the extract thread to modify this value
    QStringList                  codecs;
    ExtractThread                thread;

    static QVariantHash          dec_opt_framedrop;
    static QVariantHash          dec_opt_normal;
};

QVariantHash VideoFrameExtractorPrivate::dec_opt_framedrop;
QVariantHash VideoFrameExtractorPrivate::dec_opt_normal;

VideoFrameExtractor::VideoFrameExtractor(QObject* const parent)
    : QObject(parent)
{
    DPTR_D(VideoFrameExtractor);

    d.thread.start();
}

void VideoFrameExtractor::setSource(const QString& url)
{
    DPTR_D(VideoFrameExtractor);

    if (url == d.source)
        return;

    d.source    = url;
    d.has_video = true;

    Q_EMIT sourceChanged();

    d.safeReleaseResource();
}

QString VideoFrameExtractor::source() const
{
    return d_func().source;
}

void VideoFrameExtractor::setAsync(bool value)
{
    DPTR_D(VideoFrameExtractor);

    if (d.async == value)
        return;

    d.async = value;

    Q_EMIT asyncChanged();
}

bool VideoFrameExtractor::async() const
{
    return d_func().async;
}

void VideoFrameExtractor::setAutoExtract(bool value)
{
    DPTR_D(VideoFrameExtractor);

    if (d.auto_extract == value)
        return;

    d.auto_extract = value;

    Q_EMIT autoExtractChanged();
}

bool VideoFrameExtractor::autoExtract() const
{
    return d_func().auto_extract;
}

void VideoFrameExtractor::setPosition(qint64 value)
{
    DPTR_D(VideoFrameExtractor);

    if (!d.has_video)
        return;

    if (qAbs(value - d.position) < precision())
    {
        return;
    }

    d.position = value;

    Q_EMIT positionChanged();

    if (!autoExtract())
        return;

    extract();
}

qint64 VideoFrameExtractor::position() const
{
    return d_func().position;
}

void VideoFrameExtractor::setPrecision(int value)
{
    DPTR_D(VideoFrameExtractor);

    if (d.precision == value)
        return;

    d.auto_precision = (value < 0);

    // explain why value (p0) is used but not the actual decoded position (p)
    // it's key frame finding rule

    if (value >= 0)
    {
        d.precision = value;
    }

    Q_EMIT precisionChanged();
}

int VideoFrameExtractor::precision() const
{
    return d_func().precision;
}

void VideoFrameExtractor::extract()
{
    DPTR_D(VideoFrameExtractor);

    if (!d.async)
    {
        extractInternal(position());

        return;
    }

    class Q_DECL_HIDDEN ExtractTask : public QRunnable
    {
    public:

        ExtractTask(VideoFrameExtractor* const e, qint64 t)
            : extractor(e),
              position (t)
        {
        }

        void run()
        {
            extractor->extractInternal(position);
        }

    private:

        VideoFrameExtractor* extractor = nullptr;
        qint64 position                = 0;

    private:

        Q_DISABLE_COPY(ExtractTask);
    };

    // We want to abort the previous extract() since we are
    // only interested in the latest request.
    // So, abort_seek is a 'previous frame extract abort mechanism'
    // -- this flag is repeatedly checked by extractInPrecision()
    // (called by extractInternal()) and if true, method returns early.
    // Note if seek/decode is aborted, aborted() signal will be Q_EMITted.

    d.abort_seek = true;
    d.thread.addTask(new ExtractTask(this, position()));
}

void VideoFrameExtractor::extractInternal(qint64 pos)
{
    DPTR_D(VideoFrameExtractor);

    int precision_old = precision();

    if (!d.checkAndOpen())
    {
        Q_EMIT error(QLatin1String("Cannot open file"));
/*
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("can not open decoder...");
*/
        return;
    }

    if (precision_old != precision())
    {
        Q_EMIT precisionChanged();
    }

    bool extractOk = false;
    bool isAborted = true;
    QString err;
    extractOk      = d.extractInPrecision(pos, precision(), err, isAborted);

    if (!extractOk)
    {
        if (isAborted)
        {
            Q_EMIT aborted(QString().asprintf("Abort at position %lld: %s",
                                              pos, err.toLatin1().constData()));
        }
        else
        {
            Q_EMIT error(QString().asprintf("Cannot extract frame at position %lld: %s",
                                            pos, err.toLatin1().constData()));
        }

        return;
    }

    Q_EMIT frameExtracted(d.frame);
}

} // namespace QtAV

#include "VideoFrameExtractor.moc"
