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

#ifndef QTAV_EXAMPLE_VIDEO_WALL_H
#define QTAV_EXAMPLE_VIDEO_WALL_H

// Qt includes

#include <QList>

// Local includes

#include "AVPlayerCore.h"
#include "WidgetRenderer.h"

class QMenu;

class VideoWall : public QObject
{
    Q_OBJECT

public:

    explicit VideoWall(QObject* const parent = nullptr);
    ~VideoWall();

    void setVideoRendererTypeString(const QString& vt);
    void setRows(int n);
    void setCols(int n);
    int  rows() const;
    int  cols() const;
    void show();
    void play(const QString& file);

public Q_SLOTS:

    void stop();
    void openLocalFile();
    void openUrl();
    void about();
    void help();

protected:

    virtual bool eventFilter(QObject*, QEvent*);
    virtual void timerEvent(QTimerEvent* e);

private Q_SLOTS:

    void changeClockType();

private:

    int                         r           = 3;
    int                         c           = 3;
    int                         timer_id    = 0;
    QtAV::AVClock*              clock       = nullptr;
    QList<QtAV::AVPlayerCore*>  players;
    QWidget*                    view        = nullptr;
    QMenu*                      menu        = nullptr;
    QString                     vid         = QString::fromLatin1("qpainter");
};

#endif // QTAV_EXAMPLE_VIDEO_WALL_H
