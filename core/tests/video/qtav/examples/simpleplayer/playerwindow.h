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

#ifndef QTAV_EXAMPLE_PLAYER_WINDOW_H
#define QTAV_EXAMPLE_PLAYER_WINDOW_H

// Qt includes

#include <QWidget>

// Local includes

#include "QtAV.h"

class QSlider;
class QPushButton;

class PlayerWindow : public QWidget
{
    Q_OBJECT

public:

    explicit PlayerWindow(QWidget* const parent = nullptr);

public Q_SLOTS:

    void openMedia();
    void seekBySlider(int value);
    void seekBySlider();
    void playPause();

private Q_SLOTS:

    void updateSlider(qint64 value);
    void updateSlider();
    void updateSliderUnit();

private:

    QtAV::VideoOutput*  m_vo;
    QtAV::AVPlayerCore* m_player;
    QSlider*            m_slider;
    QPushButton*        m_openBtn;
    QPushButton*        m_playBtn;
    QPushButton*        m_stopBtn;
    int                 m_unit;
};

#endif // QTAV_EXAMPLE_PLAYER_WINDOW_H
