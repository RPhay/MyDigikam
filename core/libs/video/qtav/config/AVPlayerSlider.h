/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2006-2010 Ricardo Villalba <rvm at escomposlinux dot org>
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// TODO: hover support (like youtube and ExMplayer timeline preview)

#ifndef QTAV_WIDGETS_AVPLAYER_SLIDER_H
#define QTAV_WIDGETS_AVPLAYER_SLIDER_H

// Qt includes

#include <QSlider>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class DIGIKAM_EXPORT AVPlayerSlider : public QSlider
{
    Q_OBJECT

public:

    explicit AVPlayerSlider(QWidget* const parent = nullptr);
    ~AVPlayerSlider();

Q_SIGNALS:

    void onEnter();
    void onLeave();
    void onHover(int pos, int value);

protected:

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
    void enterEvent(QEnterEvent*)                     override;
#else
    void enterEvent(QEvent*)                          override;
#endif

    virtual void leaveEvent(QEvent* e)                override;
    virtual void mouseMoveEvent(QMouseEvent* e)       override;
    virtual void mousePressEvent(QMouseEvent* e)      override;

    inline int pick(const QPoint& pt)           const;
    int pixelPosToRangeValue(int pos)           const;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_AVPLAYER_SLIDER_H
