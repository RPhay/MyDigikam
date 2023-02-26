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

#ifndef AV_PLAYER_PLAYLIST_ITEM_H
#define AV_PLAYER_PLAYLIST_ITEM_H

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
    qint64  mLastTime;
    qint64  mDuration;
    QString mLastTimeS;
    QString mDurationS;
};

QDataStream& operator>> (QDataStream& s, PlayListItem& p);
QDataStream& operator<< (QDataStream& s, const PlayListItem& p);

} // namespace AVPlayer

Q_DECLARE_METATYPE(AVPlayer::PlayListItem);

#endif // AV_PLAYER_PLAYLIST_ITEM_H
