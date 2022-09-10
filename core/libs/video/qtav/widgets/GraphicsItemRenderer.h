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

#ifndef QTAV_WIDGETS_GRAPHICS_ITEM_RENDERER_H
#define QTAV_WIDGETS_GRAPHICS_ITEM_RENDERER_H

// Qt includes

#include <QGraphicsWidget>

// Local includes

#include "QtAVWidgets_Global.h"
#include "QPainterRenderer.h"

// QGraphicsWidget will lose focus forever if TextItem inserted text. Why?

#define CONFIG_GRAPHICSWIDGET 0

#if CONFIG_GRAPHICSWIDGET
#   define GraphicsWidget QGraphicsWidget
#else
#   define GraphicsWidget QGraphicsObject
#endif

namespace QtAV
{

class GraphicsItemRendererPrivate;

class DIGIKAM_EXPORT GraphicsItemRenderer : public GraphicsWidget,
                                            public QPainterRenderer
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(GraphicsItemRenderer)
    Q_PROPERTY(bool opengl READ isOpenGL WRITE setOpenGL NOTIFY openGLChanged)
    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(qreal contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
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

    explicit GraphicsItemRenderer(QGraphicsItem* const parent = nullptr);

    VideoRendererId id()                              const override;
    bool isSupported(VideoFormat::PixelFormat pixfmt) const override;

    QRectF boundingRect()                             const override;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget)                             override;

    QGraphicsItem* graphicsItem()                           override
    {
        return this;
    }

    /*!
     * \brief isOpenGL
     * true: user set to enabling opengl renderering. if viewport is not GLWidget, nothing will be rendered
     * false: otherwise. opengl resources in QtAV (e.g. shader manager) will be released later
     */
    bool isOpenGL()                                   const;
    void setOpenGL(bool o);

    OpenGLVideo* opengl() const                             override;

    DECLARE_VIDEO_RENDERER_SIGNALS

Q_SIGNALS:

    void openGLChanged();

protected:

    GraphicsItemRenderer(GraphicsItemRendererPrivate& d, QGraphicsItem* const parent);

    bool receiveFrame(const VideoFrame& frame)              override;
    void drawBackground()                                   override;

    // draw the current frame using the current paint engine. called by paintEvent()

    void drawFrame()                                        override;

#if CONFIG_GRAPHICSWIDGET

    bool event(QEvent* event)                               override;

#else

    bool event(QEvent* event)                               override;

    //bool sceneEvent(QEvent *event) override;

#endif

private:

    void onSetOutAspectRatioMode(OutAspectRatioMode mode)   override;
    void onSetOutAspectRatio(qreal ratio)                   override;
    bool onSetOrientation(int value)                        override;
    bool onSetBrightness(qreal b)                           override;
    bool onSetContrast(qreal c)                             override;
    bool onSetHue(qreal h)                                  override;
    bool onSetSaturation(qreal s)                           override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS

    // Disable

    GraphicsItemRenderer(QObject*);
};

typedef GraphicsItemRenderer VideoRendererGraphicsItem;

} // namespace QtAV

#endif // QTAV_WIDGETS_GRAPHICS_ITEM_RENDERER_H
