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

#ifndef AV_PLAYER_EVENT_FILTER_H
#define AV_PLAYER_EVENT_FILTER_H

/*
 * This class is used interally as QtAV's default event filter. It is suite for single player object
 */

#include <QObject>
#include <QPoint>

QT_BEGIN_NAMESPACE
class QMenu;
class QPoint;
QT_END_NAMESPACE

namespace QtAV
{
class AVPlayerCore;
}

namespace AVPlayer
{

// for internal use

class EventFilter : public QObject
{
    Q_OBJECT

public:

    explicit EventFilter(QtAV::AVPlayerCore* const player);
    virtual ~EventFilter();

Q_SIGNALS:

    void helpRequested();
    void showNextOSD();

public Q_SLOTS:

    void openLocalFile();
    void openUrl();
    void about();
    void aboutFFmpeg();
    void help();

protected:

    virtual bool eventFilter(QObject*, QEvent*);
    void showMenu(const QPoint& p);

private:

    QMenu* menu;
};

// ---------------------------------------------------------------------------

class WindowEventFilter : public QObject
{
    Q_OBJECT

public:

    explicit WindowEventFilter(QWidget* const window);

Q_SIGNALS:

    void fullscreenChanged();

protected:

    virtual bool eventFilter(QObject* const watched, QEvent* const event);

private:

    QWidget* mpWindow;
    QPoint   gMousePos, iMousePos;
};

} // namespace AVPlayer

#endif // AV_PLAYER_EVENT_FILTER_H
