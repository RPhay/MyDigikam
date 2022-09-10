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

#ifndef QTAV_EXAMPLE_WIDGET_H
#define QTAV_EXAMPLE_WIDGET_H

// Qt includes

#include <QWidget>
#include <QPushButton>

namespace QtAV
{
class GLWidgetRenderer2;
class AVPlayerCore;
class VideoOutput;
}

class Widget : public QWidget
{
    Q_OBJECT

public:

    explicit Widget(QWidget* const parent = nullptr);
    ~Widget();

public Q_SLOTS:

    void setVideo();
    void playVideo();
    void testRTSP();

private:

    QtAV::VideoOutput*  renderer    = nullptr;
    QtAV::AVPlayerCore* player[2]   = { nullptr };
    QPushButton*        play_btn[2] = { nullptr };
    QPushButton*        file_btn[2] = { nullptr };
};

#endif // QTAV_EXAMPLE_WIDGET_H
