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

#ifndef QTAV_EXAMPLE_VIDEO_PLAYER_H
#define QTAV_EXAMPLE_VIDEO_PLAYER_H

// Qt includes

#include <QWidget>

// Local includes

#include "AVPlayerCore.h"
#include "GraphicsItemRenderer.h"


class QGraphicsView;


class VideoPlayer : public QWidget
{
    Q_OBJECT

public:

    explicit VideoPlayer(QWidget* const parent = nullptr);
    ~VideoPlayer();

    QSize sizeHint() const
    {
        return QSize(720, 640);
    }

    void play(const QString& file);

public Q_SLOTS:

    void setOpenGL(bool o = true);

private Q_SLOTS:

    void setOrientation(int value);
    void rotateVideo(int angle);
    void scaleVideo(int value);
    void open();

private:

    QtAV::AVPlayerCore          mediaPlayer;
    QtAV::GraphicsItemRenderer* videoItem;
    QGraphicsView*              view;
};

#endif // QTAV_EXAMPLE_VIDEO_PLAYER_H
