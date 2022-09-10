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

#ifndef QTAV_WIDGETS_WIDGET_RENDERER_H
#define QTAV_WIDGETS_WIDGET_RENDERER_H

// Qt includes

#include <QWidget>

// Local includes

#include "QtAVWidgets_Global.h"
#include "QPainterRenderer.h"

namespace QtAV
{

class WidgetRendererPrivate;

class DIGIKAM_EXPORT WidgetRenderer : public QWidget,
                                      public QPainterRenderer
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(WidgetRenderer)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QRectF regionOfInterest READ regionOfInterest WRITE setRegionOfInterest NOTIFY regionOfInterestChanged)
    Q_PROPERTY(qreal sourceAspectRatio READ sourceAspectRatio NOTIFY sourceAspectRatioChanged)
    Q_PROPERTY(qreal outAspectRatio READ outAspectRatio WRITE setOutAspectRatio NOTIFY outAspectRatioChanged)

    // fillMode

    // TODO: how to use enums in base class as property or Q_ENUM

    Q_PROPERTY(OutAspectRatioMode outAspectRatioMode READ outAspectRatioMode WRITE setOutAspectRatioMode NOTIFY outAspectRatioModeChanged)
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect videoRect READ videoRect NOTIFY videoRectChanged)
    Q_PROPERTY(QSize videoFrameSize READ videoFrameSize NOTIFY videoFrameSizeChanged)

public:

    explicit WidgetRenderer(QWidget* const parent = nullptr,
                            Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget));

    virtual VideoRendererId id()          const override;

    virtual QWidget* widget()                   override
    {
        return this;
    }

    DECLARE_VIDEO_RENDERER_SIGNALS

Q_SIGNALS:

    void imageReady();

protected:

    bool receiveFrame(const VideoFrame& frame)  override;
    void resizeEvent(QResizeEvent*)             override;

    /**
     * usually you don't need to reimplement paintEvent, just drawXXX() is ok. unless you want do all
     * things yourself totally
     */

    void paintEvent(QPaintEvent*)               override;

    bool onSetOrientation(int value)            override;

protected:

    WidgetRenderer(WidgetRendererPrivate& d, QWidget* const parent, Qt::WindowFlags f);

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

typedef WidgetRenderer VideoRendererWidget;

} // namespace QtAV

#endif // QTAV_WIDGETS_WIDGET_RENDERER_H
