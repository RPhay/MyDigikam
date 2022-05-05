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
#include <QtAV>
#include <QtAVWidgets>
#include <QPushButton>

class VideoGroup : public QObject
{
    Q_OBJECT
public:
    explicit VideoGroup(QObject *parent = 0);
    ~VideoGroup();
    void setVideoRendererTypeString(const QString& vt);
    void setRows(int n);
    void setCols(int n);
    int rows() const;
    int cols() const;
    void show();
    void play(const QString& file);
    void updateROI();

public slots:
    void setSingleWindow(bool s);
    void toggleSingleFrame(bool s);
    void toggleFrameless(bool f);
    void openLocalFile();
    void addRenderer();
    void removeRenderer();

private:
    bool m1Window;
    bool m1Frame;
    bool mFrameless;
    int r, c;
    int timer_id;
    QtAV::AVPlayer *mpPlayer;
    QWidget *view;
    QWidget *mpBar;
    QPushButton *mpAdd, *mpRemove, *mpOpen, *mpPlay, *mpStop, *mpPause, *mp1Window;
    QPushButton *mp1Frame;
    QPushButton *mpFrameless;
    QString vid;
    QList<QtAV::VideoRenderer*> mRenderers;
};

#endif // QTAV_VIDEOWALL_H
