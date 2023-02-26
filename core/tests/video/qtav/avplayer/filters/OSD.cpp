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

#include "OSD.h"

namespace AVPlayer
{

OSD::OSD()
    : mShowType (ShowNone),
      mSecsTotal(-1)
{
}

OSD::~OSD()
{
}

void OSD::setShowType(ShowType type)
{
    mShowType = type;
}

OSD::ShowType OSD::showType() const
{
    return mShowType;
}

void OSD::useNextShowType()
{
    if (mShowType == ShowNone)
    {
        mShowType = (ShowType)1;

        return;
    }

    if ((mShowType + 1) == ShowNone)
    {
        mShowType = ShowNone;

        return;
    }

    mShowType = (ShowType)(mShowType << 1);
}

bool OSD::hasShowType(ShowType t) const
{
    return ((t & mShowType) == t);
}

QString OSD::text(Statistics* statistics)
{
    QString text;
    Statistics::Common* av = &statistics->video;

    if (!av->available)
        av = &statistics->audio;

    if (hasShowType(ShowCurrentTime) || hasShowType(ShowCurrentAndTotalTime))
    {
        text = av->current_time.toString(QString::fromLatin1("HH:mm:ss"));
    }

    // how to compute mSecsTotal only once?

    if (hasShowType(ShowCurrentAndTotalTime) || hasShowType(ShowPercent) /*(mSecsTotal < 0)*/)
    {
        if (statistics->duration.isNull())
            return text;

        mSecsTotal = QTime(0, 0, 0).secsTo(statistics->duration); // why video.total_time may be wrong(mkv)
    }

    if (hasShowType(ShowCurrentAndTotalTime))
    {
        if (mSecsTotal > 0)
            text += QString::fromLatin1("/").append(statistics->duration.toString(QString::fromLatin1("HH:mm:ss")));
        else
            text += QString::fromLatin1("/--:--:--");
    }

    if (hasShowType(ShowRemainTime))
    {
        if (mSecsTotal > 0)
            text += QString::fromLatin1("-").append(QTime(0, 0, 0)
                    .addSecs(av->current_time.secsTo(statistics->duration)).toString(QString::fromLatin1("HH:mm:ss")));
        else
            text += QString::fromLatin1("--:--:--");
    }

    if (hasShowType(ShowPercent))
    {
        if (mSecsTotal > 0)
            text += QString::number(qreal(QTime(0, 0, 0).secsTo(av->current_time)) /
                                    qreal(mSecsTotal)*100.0, 'f', 1).append(QString::fromLatin1("%"));
        else
            text += QString::fromLatin1("--.-%");
    }

    return text;
}

} // namespace AVPlayer
