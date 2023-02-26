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

#ifndef AV_PLAYER_EVENT_FILTER_H
#define AV_PLAYER_EVENT_FILTER_H


#include <QObject>
#include <QPoint>

class QMenu;
class QPoint;

namespace QtAV
{
class AVPlayerCore;
}

/**
 * These classes are used interally as QtAV's default event filter. It is suite for single player object
 */

namespace AVPlayer
{

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
    QPoint   gMousePos;
    QPoint   iMousePos;
};

} // namespace AVPlayer

#endif // AV_PLAYER_EVENT_FILTER_H
