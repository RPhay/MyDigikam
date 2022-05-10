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

#ifndef QTAV_PLAYER_PLAYLISTITEM_H
#define QTAV_PLAYER_PLAYLISTITEM_H

// Qt includes

#include <QString>
#include <QVariant>

namespace AVPlayer
{

class PlayListItem
{
public:

    PlayListItem();

    void setTitle(const QString& title);
    QString title() const;

    void setUrl(const QString& url);
    QString url() const;

    void setStars(int s);
    int stars() const;

    void setLastTime(qint64 ms);
    qint64 lastTime() const;
    QString lastTimeString() const;

    void setDuration(qint64 ms);
    qint64 duration() const;
    QString durationString() const;

    // icon

    bool operator ==(const PlayListItem& other) const;

private:

    QString mTitle;
    QString mUrl;
    int     mStars;
    qint64  mLastTime, mDuration;
    QString mLastTimeS, mDurationS;
};

QDataStream& operator>> (QDataStream& s, PlayListItem& p);
QDataStream& operator<< (QDataStream& s, const PlayListItem& p);

} // namespace AVPlayer

Q_DECLARE_METATYPE(QtAVPlayer::PlayListItem);

#endif // QTAV_PLAYER_PLAYLISTITEM_H
