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

#ifndef QTAV_VIDEO_OUTPUT_H
#define QTAV_VIDEO_OUTPUT_H

// Qt includes

#include <QObject>

// Local includes

#include "VideoRenderer.h"

namespace QtAV
{

class VideoOutputPrivate;

/*!
 * \brief The VideoOutput class
 * A VideoRenderer wrapper with QObject features.
 * If create VideoOutput without a given renderer id,
 * QtAV will try to create a widget based renderer,
 * and dynamically load QtAVWidgets module if it's not loaded.
 */
class DIGIKAM_EXPORT VideoOutput : public QObject,
                                   public VideoRenderer
{
    DPTR_DECLARE_PRIVATE(VideoOutput)
    Q_OBJECT
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

    /*!
     * \brief VideoOutput
     * Create a QWidget based renderer. Result can be a QOpenGLWidget
     * or QGLWidget based renderer if possible.
     * Otherwise fallback to a software renderer
     */
    explicit VideoOutput(QObject* const parent = nullptr);

    /*!
     * \brief VideoOutput
     * Create a renderer with given rendererId.
     * MUST check VideoOutput::isAvailable() later!
     */
    explicit VideoOutput(VideoRendererId rendererId, QObject* const parent = nullptr);
    ~VideoOutput();

    VideoRendererId id()                                              const override;

    VideoFormat::PixelFormat preferredPixelFormat()                   const override;
    bool isSupported(VideoFormat::PixelFormat pixfmt)                 const override;
    QWindow* qwindow()                                                      override final;
    QWidget* widget()                                                       override final;
    QGraphicsItem* graphicsItem()                                           override final;
    OpenGLVideo* opengl()                                             const override;

    DECLARE_VIDEO_RENDERER_SIGNALS

protected:

    bool eventFilter(QObject* obj, QEvent* event)                           override;
    bool receiveFrame(const VideoFrame& frame)                              override;
    void drawBackground()                                                   override;
    void drawFrame()                                                        override;
    void handlePaintEvent()                                                 override;

private:

    virtual bool onSetPreferredPixelFormat(VideoFormat::PixelFormat pixfmt) override;
    virtual bool onForcePreferredPixelFormat(bool force = true)             override;
    virtual void onSetOutAspectRatioMode(OutAspectRatioMode mode)           override;
    virtual void onSetOutAspectRatio(qreal ratio)                           override;
    virtual bool onSetQuality(Quality q)                                    override;
    virtual bool onSetOrientation(int value)                                override;
    virtual void onResizeRenderer(int width, int height)                    override;
    virtual bool onSetRegionOfInterest(const QRectF& roi)                   override;
    virtual QPointF onMapToFrame(const QPointF& p)                    const override;
    virtual QPointF onMapFromFrame(const QPointF& p)                  const override;

    virtual bool onSetBrightness(qreal brightness)                          override;
    virtual bool onSetContrast(qreal contrast)                              override;
    virtual bool onSetHue(qreal hue)                                        override;
    virtual bool onSetSaturation(qreal saturation)                          override;
    virtual void onSetBackgroundColor(const QColor& color)                  override;

    // from AVOutput

    // called by friend AVPlayerCore
    virtual void setStatistics(Statistics* statistics)                      override;

    virtual bool onInstallFilter(Filter* filter, int index)                 override;
    virtual bool onUninstallFilter(Filter* filter)                          override;
    virtual bool onHanlePendingTasks()                                      override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

} // namespace QtAV

#endif // QTAV_VIDEO_OUTPUT_H
