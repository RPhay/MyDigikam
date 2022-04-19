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

#include <QObject>
#include "VideoRenderer.h"

namespace QtAV
{

class VideoOutputPrivate;

/*!
 * \brief The VideoOutput class
 * A VideoRenderer wrapper with QObject features. If create VideoOutput without a given renderer id, QtAV will try to create a widget based renderer, and dynamically load QtAVWidgets module if it's not loaded.
 */
class Q_AV_EXPORT VideoOutput : public QObject,
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
    VideoOutput(QObject *parent = 0);

    /*!
     * \brief VideoOutput
     * Create a renderer with given rendererId. MUST check VideoOutput::isAvailable() later!
     */
    VideoOutput(VideoRendererId rendererId, QObject *parent = 0);
    ~VideoOutput();
    VideoRendererId id() const Q_DECL_OVERRIDE;

    VideoFormat::PixelFormat preferredPixelFormat() const Q_DECL_OVERRIDE;
    bool isSupported(VideoFormat::PixelFormat pixfmt) const Q_DECL_OVERRIDE;
    QWindow* qwindow() Q_DECL_OVERRIDE Q_DECL_FINAL;
    QWidget* widget() Q_DECL_OVERRIDE Q_DECL_FINAL;
    QGraphicsItem* graphicsItem() Q_DECL_OVERRIDE Q_DECL_FINAL;
    OpenGLVideo* opengl() const Q_DECL_OVERRIDE;

Q_SIGNALS:

    void sourceAspectRatioChanged(qreal value) Q_DECL_OVERRIDE Q_DECL_FINAL;
    void regionOfInterestChanged() Q_DECL_OVERRIDE;
    void outAspectRatioChanged() Q_DECL_OVERRIDE;
    void outAspectRatioModeChanged() Q_DECL_OVERRIDE;
    void brightnessChanged(qreal value) Q_DECL_OVERRIDE;
    void contrastChanged(qreal) Q_DECL_OVERRIDE;
    void hueChanged(qreal) Q_DECL_OVERRIDE;
    void saturationChanged(qreal) Q_DECL_OVERRIDE;
    void backgroundColorChanged() Q_DECL_OVERRIDE;
    void orientationChanged() Q_DECL_OVERRIDE;
    void videoRectChanged() Q_DECL_OVERRIDE;
    void videoFrameSizeChanged() Q_DECL_OVERRIDE;

protected:

    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    bool receiveFrame(const VideoFrame& frame) Q_DECL_OVERRIDE;
    void drawBackground() Q_DECL_OVERRIDE;
    void drawFrame() Q_DECL_OVERRIDE;
    void handlePaintEvent() Q_DECL_OVERRIDE;

private:

    virtual bool onSetPreferredPixelFormat(VideoFormat::PixelFormat pixfmt) Q_DECL_OVERRIDE;
    virtual bool onForcePreferredPixelFormat(bool force = true) Q_DECL_OVERRIDE;
    virtual void onSetOutAspectRatioMode(OutAspectRatioMode mode) Q_DECL_OVERRIDE;
    virtual void onSetOutAspectRatio(qreal ratio) Q_DECL_OVERRIDE;
    virtual bool onSetQuality(Quality q) Q_DECL_OVERRIDE;
    virtual bool onSetOrientation(int value) Q_DECL_OVERRIDE;
    virtual void onResizeRenderer(int width, int height) Q_DECL_OVERRIDE;
    virtual bool onSetRegionOfInterest(const QRectF& roi) Q_DECL_OVERRIDE;
    virtual QPointF onMapToFrame(const QPointF& p) const Q_DECL_OVERRIDE;
    virtual QPointF onMapFromFrame(const QPointF& p) const Q_DECL_OVERRIDE;

    virtual bool onSetBrightness(qreal brightness) Q_DECL_OVERRIDE;
    virtual bool onSetContrast(qreal contrast) Q_DECL_OVERRIDE;
    virtual bool onSetHue(qreal hue) Q_DECL_OVERRIDE;
    virtual bool onSetSaturation(qreal saturation) Q_DECL_OVERRIDE;
    virtual void onSetBackgroundColor(const QColor& color) Q_DECL_OVERRIDE;

    // from AVOutput

    virtual void setStatistics(Statistics* statistics) Q_DECL_OVERRIDE; // called by friend AVPlayer
    virtual bool onInstallFilter(Filter *filter, int index) Q_DECL_OVERRIDE;
    virtual bool onUninstallFilter(Filter *filter) Q_DECL_OVERRIDE;
    virtual bool onHanlePendingTasks() Q_DECL_OVERRIDE;
};

} // namespace QtAV

#endif // QTAV_VIDEO_OUTPUT_H
