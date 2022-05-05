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



#ifndef QTAV_VIDEOWALL_H
#define QTAV_VIDEOWALL_H

#include <QList>
#include <QtAV/AVPlayer.h>
#include <QtAVWidgets/WidgetRenderer.h>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE
class VideoWall : public QObject
{
    Q_OBJECT
public:
    explicit VideoWall(QObject *parent = 0);
    ~VideoWall();
    void setVideoRendererTypeString(const QString& vt);
    void setRows(int n);
    void setCols(int n);
    int rows() const;
    int cols() const;
    void show();
    void play(const QString& file);

public slots:
    void stop();
    void openLocalFile();
    void openUrl();
    void about();
    void help();

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void timerEvent(QTimerEvent *e);

private Q_SLOTS:
    void changeClockType();

private:
    int r, c;
    int timer_id;
    QtAV::AVClock *clock;
    QList<QtAV::AVPlayer*> players;
    QWidget *view;
    QMenu *menu;
    QString vid;
};

#endif // QTAV_VIDEOWALL_H
