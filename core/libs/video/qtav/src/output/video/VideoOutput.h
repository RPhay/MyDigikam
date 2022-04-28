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
 * A VideoRenderer wrapper with QObject features. If create VideoOutput without a given renderer id, QtAV will try to create a widget based renderer, and dynamically load QtAVWidgets module if it's not loaded.
 */
class QTAV_EXPORT VideoOutput : public QObject,
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
    Q_ENUMS(OutAspectRatioMode)
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect videoRect READ videoRect NOTIFY videoRectChanged)
    Q_PROPERTY(QSize videoFrameSize READ videoFrameSize NOTIFY videoFrameSizeChanged)
    Q_ENUMS(Quality)

public:

    /*!
     * \brief VideoOutput
     * Create a QWidget based renderer. Result can be a QOpenGLWidget or QGLWidget based renderer if possible. Otherwise fallback to a software renderer
     */
    VideoOutput(QObject *parent = nullptr);

    /*!
     * \brief VideoOutput
     * Create a renderer with given rendererId. MUST check VideoOutput::isAvailable() later!
     */
    VideoOutput(VideoRendererId rendererId, QObject *parent = nullptr);
    ~VideoOutput();
    VideoRendererId id() const                                              override;

    VideoFormat::PixelFormat preferredPixelFormat() const                   override;
    bool isSupported(VideoFormat::PixelFormat pixfmt) const                 override;
    QWindow* qwindow()                                                      override final;
    QWidget* widget()                                                       override final;
    QGraphicsItem* graphicsItem()                                           override final;
    OpenGLVideo* opengl() const                                             override;

Q_SIGNALS:

    void sourceAspectRatioChanged(qreal value)                              override final;
    void regionOfInterestChanged()                                          override;
    void outAspectRatioChanged()                                            override;
    void outAspectRatioModeChanged()                                        override;
    void brightnessChanged(qreal value)                                     override;
    void contrastChanged(qreal)                                             override;
    void hueChanged(qreal)                                                  override;
    void saturationChanged(qreal)                                           override;
    void backgroundColorChanged()                                           override;
    void orientationChanged()                                               override;
    void videoRectChanged()                                                 override;
    void videoFrameSizeChanged()                                            override;

protected:

    bool eventFilter(QObject *obj, QEvent *event)                           override;
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
    virtual QPointF onMapToFrame(const QPointF& p) const                    override;
    virtual QPointF onMapFromFrame(const QPointF& p) const                  override;

    virtual bool onSetBrightness(qreal brightness)                          override;
    virtual bool onSetContrast(qreal contrast)                              override;
    virtual bool onSetHue(qreal hue)                                        override;
    virtual bool onSetSaturation(qreal saturation)                          override;
    virtual void onSetBackgroundColor(const QColor& color)                  override;

    // from AVOutput

    virtual void setStatistics(Statistics* statistics)                      override; // called by friend AVPlayer
    virtual bool onInstallFilter(Filter *filter, int index)                 override;
    virtual bool onUninstallFilter(Filter *filter)                          override;
    virtual bool onHanlePendingTasks()                                      override;
};

} // namespace QtAV

#endif // QTAV_VIDEO_OUTPUT_H
