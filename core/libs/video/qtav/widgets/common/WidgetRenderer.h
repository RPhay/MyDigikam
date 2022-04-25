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

#ifndef QTAV_WIDGETS_WIDGET_RENDERER_H
#define QTAV_WIDGETS_WIDGET_RENDERER_H

// Local includes

#include "QtAVWidgets_Global.h"
#include "QPainterRenderer.h"

// Qt includes

#include <QWidget>

namespace QtAV
{

class WidgetRendererPrivate;

class QTAV_WIDGETS_EXPORT WidgetRenderer : public QWidget,
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
    Q_ENUMS(OutAspectRatioMode)
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect videoRect READ videoRect NOTIFY videoRectChanged)
    Q_PROPERTY(QSize videoFrameSize READ videoFrameSize NOTIFY videoFrameSizeChanged)
    Q_ENUMS(Quality)

public:

    explicit WidgetRenderer(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget));

    virtual VideoRendererId id()          const override;

    virtual QWidget* widget()                   override
    {
        return this;
    }

Q_SIGNALS:

    void sourceAspectRatioChanged(qreal value)  override final;
    void regionOfInterestChanged()              override;
    void outAspectRatioChanged()                override;
    void outAspectRatioModeChanged()            override;
    void brightnessChanged(qreal value)         override;
    void contrastChanged(qreal)                 override;
    void hueChanged(qreal)                      override;
    void saturationChanged(qreal)               override;
    void backgroundColorChanged()               override;
    void orientationChanged()                   override;
    void videoRectChanged()                     override;
    void videoFrameSizeChanged()                override;

    void imageReady();

protected:

    bool receiveFrame(const VideoFrame& frame)  override;
    void resizeEvent(QResizeEvent*)             override;

    /* usually you don't need to reimplement paintEvent, just drawXXX() is ok. unless you want do all
     * things yourself totally
     */

    void paintEvent(QPaintEvent*)               override;

    bool onSetOrientation(int value)            override;

protected:

    WidgetRenderer(WidgetRendererPrivate& d, QWidget* parent, Qt::WindowFlags f);
};

typedef WidgetRenderer VideoRendererWidget;

} // namespace QtAV

#endif // QTAV_WIDGETS_WIDGET_RENDERER_H
