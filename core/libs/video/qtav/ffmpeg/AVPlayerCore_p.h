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

#ifndef QTAV_AVPLAYER_CORE_P_H
#define QTAV_AVPLAYER_CORE_P_H

#include "AVPlayerCore.h"

// Local includes

#include "AVDemuxer.h"
#include "AVCompat.h"
#include "AudioThread.h"
#include "VideoThread.h"
#include "AVDemuxThread.h"
#include "digikam_debug.h"

namespace QtAV
{

static const qint64 kInvalidPosition = std::numeric_limits<qint64>::max();

class Q_DECL_HIDDEN AVPlayerCore::Private
{
public:

    Private();
    ~Private();

public:

    bool checkSourceChange();
    void updateNotifyInterval();
    void applyFrameRate();
    void initStatistics();
    void initBaseStatistics();

#ifndef HAVE_FFMPEG_VERSION5

    void initCommonStatistics(int s, Statistics::Common* st, AVCodecContext* avctx);

#else // ffmpeg >= 5


    void initCommonStatistics(int s, Statistics::Common* st, AVCodecParameters* avctx);

#endif

    void initAudioStatistics(int s);
    void initVideoStatistics(int s);
    void initSubtitleStatistics(int s);
    QVariantList getTracksInfo(AVDemuxer* demuxer, AVDemuxer::StreamType st);

    bool applySubtitleStream(int n, AVPlayerCore* player);
    bool setupAudioThread(AVPlayerCore* player);
    bool setupVideoThread(AVPlayerCore* player);
    bool tryApplyDecoderPriority(AVPlayerCore* player);

    // TODO: what if buffer mode changed during playback?

    void updateBufferValue(PacketBuffer* buf);
    void updateBufferValue();

    //TODO: addAVOutput()

    template<class Out>

    void setAVOutput(Out*& pOut, Out* pNew, AVThread* thread)
    {
        Out* old        = pOut;
        bool delete_old = false;

        if (pOut == pNew)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("output not changed: %p", pOut);

            if (thread && (thread->output() == pNew))
            {
                // avthread already set that output

                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("avthread already set that output");

                return;
            }
        }
        else
        {
            pOut       = pNew;
            delete_old = true;
        }

        if (!thread)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("avthread not ready. can not set output.");

            // no avthread, we can delete it safely
            // AVOutput must be allocated in heap. Just like QObject's children.

            if (delete_old)
            {
                delete old;
                old = nullptr;
            }

            return;
        }

        // FIXME: what if isPaused()==false but pause(true) in another thread?
/*
        bool need_lock = isPlaying() && !thread->isPaused();

        if (need_lock)
            thread->lock();
*/
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("set AVThread output");

        thread->setOutput(pOut);

        if (pOut)
        {
            pOut->setStatistics(&statistics);
/*
            if (need_lock)
                thread->unlock(); // why here?
*/
        }

        // now the old avoutput is not used by avthread, we can delete it safely

        // AVOutput must be allocated in heap. Just like QObject's children.

        if (delete_old)
        {
            delete old;
            old = nullptr;
        }
    }

public:

    bool                    auto_load;
    bool                    async_load;

    // can be QString, QIODevice*

    QVariant                current_source;
    QVariant                pendding_source;

    bool                    loaded;                                     ///< for current source
    bool                    relative_time_mode;
    qint64                  media_start_pts;                            ///< read from media stream
    qint64                  media_end;
    bool                    reset_state;
    qint64                  start_position;
    qint64                  stop_position;
    qint64                  start_position_norm;                        ///< real position
    qint64                  stop_position_norm;                         ///< real position
    qint64                  last_known_good_pts;
    bool                    was_stepping;
    int                     repeat_max;
    int                     repeat_current;
    int                     timer_id;                                   ///< notify position change and check AB repeat range. active when playing

    int                     audio_track;
    int                     video_track;
    int                     subtitle_track;
    QVariantList            subtitle_tracks;
    QVariantList            video_tracks;
    QString                 external_audio;
    AVDemuxer               audio_demuxer;
    QVariantList            external_audio_tracks;
    QVariantList            audio_tracks;
    BufferMode              buffer_mode;
    qint64                  buffer_value;

    // the following things are required and must be set not null

    AVDemuxer               demuxer;
    AVDemuxThread*          read_thread;
    AVClock*                clock;
    VideoRenderer*          vo;                                         // list? TODO: remove
    AudioOutput*            ao;                                         // TODO: remove
    AudioDecoder*           adec;
    VideoDecoder*           vdec;
    AudioThread*            athread;
    VideoThread*            vthread;

    VideoCapture*           vcapture;
    Statistics              statistics;
    qreal                   speed;
    OutputSet*              vos;
    OutputSet*              aos;
    QVector<VideoDecoderId> vc_ids;
    int                     brightness;
    int                     contrast;
    int                     saturation;

    QVariantHash            ac_opt;
    QVariantHash            vc_opt;

    bool                    seeking;
    SeekType                seek_type;
    qint64                  interrupt_timeout;

    qreal                   force_fps;

    /**
     * timerEvent interval in ms. can divide 1000. depends on media duration, fps etc.
     * <0: auto compute internally, |notify_interval| is the real interval
     */
    int                     notify_interval;

    MediaStatus             status;                                     ///< status changes can be from demuxer or demux thread
    AVPlayerCore::State     state;
    MediaEndAction          end_action;
    QMutex                  load_mutex;

private:

    Q_DISABLE_COPY(Private);
};

} // namespace QtAV

#endif // QTAV_AVPLAYER_CORE_P_H
