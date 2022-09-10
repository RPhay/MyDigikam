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

#ifndef QTAV_AV_CLOCK_H
#define QTAV_AV_CLOCK_H

// Qt includes

#include <QAtomicInt>
#include <QBasicTimer>
#include <QObject>
#include <QElapsedTimer>

// Local includes

#include "QtAV_Global.h"

/**
 * AVClock is created by AVPlayerCore. The only way to access AVClock is through AVPlayerCore::masterClock()
 * The default clock type is Audio's clock, i.e. vedio synchronizes to audio. If audio stream is not
 * detected, then the clock will set to External clock automatically.
 * I name it ExternalClock because the clock can be corrected outside, though it is a clock inside AVClock
 */

namespace QtAV
{

static const double kThousandth = 0.001;

class DIGIKAM_EXPORT AVClock : public QObject
{
    Q_OBJECT

public:

    typedef enum
    {
        AudioClock,
        ExternalClock,
        VideoClock     ///< sync to video timestamp
    } ClockType;

    explicit AVClock(ClockType c, QObject* const parent = nullptr);
    explicit AVClock(QObject* const parent = nullptr);

    void setClockType(ClockType ct);
    ClockType clockType() const;
    bool isActive() const;

    /*!
     * \brief setInitialValue
     * Usually for ExternalClock. For example, media start time is not 0, clock have to set initial value as media start time
     */
    void setInitialValue(double v);
    double initialValue() const;

    /**
     * auto clock: use audio clock if audio stream found, otherwise use external clock
     */
    void setClockAuto(bool a);
    bool isClockAuto() const;

    /**
     * in seconds
     */
    inline double pts() const;

    /*!
     * \brief value
     * the real timestamp in seconds: pts + delay
     * \return
     */
    inline double value() const;
    inline void updateValue(double pts); // update the pts

    /**
     * used when seeking and correcting from external
     */
    void updateExternalClock(qint64 msecs);

    /**
     * external clock outside still running, so it's more accurate for syncing multiple clocks serially
     */
    void updateExternalClock(const AVClock& clock);

    inline void updateVideoTime(double pts);
    inline double videoTime() const;
    inline double delay() const;    // playing audio spends some time
    inline void updateDelay(double delay);
    inline qreal diff() const;

    void setSpeed(qreal speed);
    inline qreal speed() const;

    bool isPaused() const;

    /*!
     * \brief syncStart
     * For internal use now
     * Start to sync "count" objects. Call syncEndOnce(id) "count" times to end sync.
     * \param count Number of objects to sync. Each one should call syncEndOnce(int id)
     * \return an id
     */
    int syncStart(int count);

    int syncId() const
    {
        return sync_id;
    }

    /*!
     * \brief syncEndOnce
     * Decrease sync objects count if id is current sync id.
     * \return true if sync is end for id or id is not current sync id
     */
    bool syncEndOnce(int id);

Q_SIGNALS:

    void paused(bool);
    void paused();      // equals to paused(true)
    void resumed();     // equals to paused(false)
    void started();
    void resetted();    // krazy:exclude=spelling

public Q_SLOTS:

    // these slots are not frequently used. so not inline

    /**
     *start the external clock
     */
    void start();

    /**
     * pause external clock
     */
    void pause(bool p);

    /**
     * reset clock initial value and external clock parameters (and stop timer). keep speed() and isClockAuto()
     */
    void reset();

protected:

    virtual void timerEvent(QTimerEvent* event);

private Q_SLOTS:

    /**
     * make sure QBasic timer start/stop in a right thread
     */
    void restartCorrectionTimer();
    void stopCorrectionTimer();

private:

    bool                    auto_clock;
    int                     m_state;
    ClockType               clock_type;
    mutable double          pts_;
    mutable double          pts_v;
    double                  delay_;
    mutable QElapsedTimer   timer;
    qreal                   mSpeed;
    double                  value0;

    /*!
     * \brief correction_schedule_timer
     * accumulative error is too large using QElapsedTimer.restart() frequently.
     * we periodically correct value() to keep the error always less
     * than the error of calling QElapsedTimer.restart() once
     * see github issue 46, 307 etc
     */
    QBasicTimer             correction_schedule_timer;

    qint64                  t;                          ///< absolute time for elapsed timer correction
    static const int        kCorrectionInterval = 1;    ///< 1000ms
    double                  last_pts;
    double                  avg_err;                    ///< average error of restart()
    mutable int             nb_restarted;
    QAtomicInt              nb_sync;
    int                     sync_id;
};

double AVClock::value() const
{
    if      (clock_type == AudioClock)
    {
        // TODO: audio clock need a timer too
        // timestamp from media stream is >= value0

        return ((pts_ == 0) ? value0 : pts_ + delay_);
    }
    else if (clock_type == ExternalClock)
    {
        if (timer.isValid())
        {
            ++nb_restarted;
            pts_ += (double(timer.restart()) * kThousandth + avg_err) * speed();
        }
        else
        {
            // timer is paused
/*
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("clock is paused. return the last value %f", pts_);
*/
        }

        return (pts_ + value0);
    }
    else
    {
        return pts_v; // value0 is 1st video pts_v already
    }
}

void AVClock::updateValue(double pts)
{
    if (clock_type == AudioClock)
        pts_ = pts;
}

void AVClock::updateVideoTime(double pts)
{
    pts_v = pts;

    if (clock_type == VideoClock)
        timer.restart();
}

double AVClock::videoTime() const
{
    return pts_v;
}

double AVClock::delay() const
{
    return delay_;
}

void AVClock::updateDelay(double delay)
{
    delay_ = delay;
}

qreal AVClock::diff() const
{
    return value() - videoTime();
}

qreal AVClock::speed() const
{
    return mSpeed;
}

} // namespace QtAV

#endif // QTAV_AV_CLOCK_H
