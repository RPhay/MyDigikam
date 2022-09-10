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

#define CODE_FOR_CLICK 1 // 0 = old code, 1 = code copied from QAVPlayerSlider, 2 = button swap

#include "AVPlayerSlider.h"

// Qt includes

#include <QApplication>
#include <QMouseEvent>

#if CODE_FOR_CLICK <= 1
#   include <QStyle>
#   if CODE_FOR_CLICK == 1
#       include <QStyleOption>
#   endif
#endif

// Local includes

#include "digikam_debug.h"

namespace QtAV
{

AVPlayerSlider::AVPlayerSlider(QWidget* const parent)
    : QSlider(parent)
{
    setOrientation(Qt::Horizontal);
    setMouseTracking(true); // mouseMoveEvent without press.
}

AVPlayerSlider::~AVPlayerSlider()
{
}

#if CODE_FOR_CLICK == 1

// Function copied from qslider.cpp

inline int AVPlayerSlider::pick(const QPoint& pt) const
{
    return (orientation() == Qt::Horizontal ? pt.x() : pt.y());
}

// Function copied from qslider.cpp and modified to make it compile

int AVPlayerSlider::pixelPosToRangeValue(int pos) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    if (orientation() == Qt::Horizontal)
    {
        sliderLength = sr.width();
        sliderMin    = gr.x();
        sliderMax    = gr.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = sr.height();
        sliderMin    = gr.y();
        sliderMax    = gr.bottom() - sliderLength + 1;
    }

    return QStyle::sliderValueFromPosition(minimum(), maximum(), pos - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
void AVPlayerSlider::enterEvent(QEnterEvent* e)
#else
void AVPlayerSlider::enterEvent(QEvent* e)
#endif
{
    Q_EMIT onEnter();

    QSlider::enterEvent(e);
}

void AVPlayerSlider::leaveEvent(QEvent* e)
{
    Q_EMIT onLeave();

    QSlider::leaveEvent(e);
}

void AVPlayerSlider::mouseMoveEvent(QMouseEvent* e)
{
    const int o = style()->pixelMetric(QStyle::PM_SliderLength ) - 1;
    int v       = QStyle::sliderValueFromPosition(minimum(), maximum(), e->pos().x() - o / 2, width() - o, false);

    Q_EMIT onHover(e->x(), v);

    QSlider::mouseMoveEvent(e);
}

// Based on code from qslider.cpp

void AVPlayerSlider::mousePressEvent(QMouseEvent* e)
{
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
        << QString::asprintf("pressed (%d, %d)", e->pos().x(), e->pos().y());

    if (e->button() == Qt::LeftButton)
    {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        const QPoint center    = sliderRect.center() - sliderRect.topLeft();

        // to take half of the slider off for the setSliderPosition call we use the center - topLeft

        if (!sliderRect.contains(e->pos()))
        {
            qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("accept");
            e->accept();

            int v = pixelPosToRangeValue(pick(e->pos() - center));
            setSliderPosition(v);
            triggerAction(SliderMove);
            setRepeatAction(SliderNoAction);

            Q_EMIT sliderMoved(v);    // TODO: ok?
            Q_EMIT sliderPressed();   // TODO: ok?
        }
        else
        {
            QSlider::mousePressEvent(e);
        }
    }
    else
    {
        QSlider::mousePressEvent(e);
    }
}

#endif // CODE_FOR_CLICK == 1

#if CODE_FOR_CLICK == 2

void AVPlayerSlider::mousePressEvent(QMouseEvent* e)
{
    // Swaps middle button click with left click

    if      (e->button() == Qt::LeftButton)
    {
        QMouseEvent ev2(QEvent::MouseButtonRelease, e->pos(), e->globalPos(),
                        Qt::MidButton, Qt::MidButton, e->modifiers());
        QSlider::mousePressEvent(&ev2);
    }
    else if (e->button() == Qt::MidButton)
    {
        QMouseEvent ev2(QEvent::MouseButtonRelease, e->pos(), e->globalPos(),
                        Qt::LeftButton, Qt::LeftButton, e->modifiers());
        QSlider::mousePressEvent(&ev2);
    }
    else
    {
        QSlider::mousePressEvent(e);
    }
}

#endif // CODE_FOR_CLICK == 2

#if CODE_FOR_CLICK == 0

void AVPlayerSlider::mousePressEvent(QMouseEvent* e)
{
    // FIXME:
    // The code doesn't work well with right to left layout,
    // so it's disabled.

    if (qApp->isRightToLeft())
    {
        QSlider::mousePressEvent(e);

        return;
    }

    int range = maximum()-minimum();
    int pos   = (e->x() * range) / width();
/*
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf( "width: %d x: %d", width(), e->x());
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf( "range: %d pos: %d value: %d", range, pos, value());
*/
    // Calculate how many positions takes the slider handle

    int metric                  = qApp->style()->pixelMetric(QStyle::PM_SliderLength);
    double one_tick_pixels      = (double)width() / range;
    int slider_handle_positions = (int)(metric / one_tick_pixels);

/*
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("metric: %d", metric );
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("one_tick_pixels :%f", one_tick_pixels);
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("width() :%d", width());
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("slider_handle_positions: %d", slider_handle_positions);
*/

    if (abs(pos - value()) > slider_handle_positions)
    {
        setValue(pos);

        Q_EMIT sliderMoved(pos);
    }
    else
    {
        QSlider::mousePressEvent(e);
    }
}

#endif

} // namespace QtAV
