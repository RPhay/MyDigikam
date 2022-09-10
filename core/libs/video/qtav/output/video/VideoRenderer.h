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

#ifndef QTAV_VIDEO_RENDERER_H
#define QTAV_VIDEO_RENDERER_H

// Qt includes

#include <QByteArray>
#include <QSize>
#include <QRectF>
#include <QColor>
#include <QObject>

// Local includes

#include "AVOutput.h"
#include "VideoFrame.h"

/**
 " property change: helper macro to declare signals in subclasses.
 */
#define DECLARE_VIDEO_RENDERER_SIGNALS          \
Q_SIGNALS:                                      \
    void sourceAspectRatioChanged(qreal value); \
    void regionOfInterestChanged();             \
    void outAspectRatioChanged();               \
    void outAspectRatioModeChanged();           \
    void brightnessChanged(qreal value);        \
    void contrastChanged(qreal);                \
    void hueChanged(qreal);                     \
    void saturationChanged(qreal);              \
    void backgroundColorChanged();              \
    void orientationChanged();                  \
    void videoRectChanged();                    \
    void videoFrameSizeChanged();               \
    void contentRectChanged();                  \
    void rendererSizeChanged();

/**
 " property change: helper macro to declare pure virtual emit methods in subclasses.
 */
#define DECLARE_VIDEO_RENDERER_EMIT_METHODS             \
private:                                                \
    void emitSourceAspectRatioChanged(qreal) override;  \
    void emitOutAspectRatioChanged()         override;  \
    void emitOutAspectRatioModeChanged()     override;  \
    void emitOrientationChanged()            override;  \
    void emitVideoRectChanged()              override;  \
    void emitContentRectChanged()            override;  \
    void emitRegionOfInterestChanged()       override;  \
    void emitVideoFrameSizeChanged()         override;  \
    void emitRendererSizeChanged()           override;  \
    void emitBrightnessChanged(qreal)        override;  \
    void emitContrastChanged(qreal)          override;  \
    void emitHueChanged(qreal)               override;  \
    void emitSaturationChanged(qreal)        override;  \
    void emitBackgroundColorChanged()        override;

/**
 " property change: helper macro to re-implement pure virtual emit methods in subclasses.
 */
#define IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(ClassName)                                          \
void ClassName::emitSourceAspectRatioChanged(qreal v) { Q_EMIT sourceAspectRatioChanged(v);   }   \
void ClassName::emitOutAspectRatioChanged()           { Q_EMIT outAspectRatioChanged();       }   \
void ClassName::emitOutAspectRatioModeChanged()       { Q_EMIT outAspectRatioModeChanged();   }   \
void ClassName::emitOrientationChanged()              { Q_EMIT orientationChanged();          }   \
void ClassName::emitVideoRectChanged()                { Q_EMIT videoRectChanged();            }   \
void ClassName::emitContentRectChanged()              { Q_EMIT contentRectChanged();          }   \
void ClassName::emitRegionOfInterestChanged()         { Q_EMIT regionOfInterestChanged();     }   \
void ClassName::emitVideoFrameSizeChanged()           { Q_EMIT videoFrameSizeChanged();       }   \
void ClassName::emitRendererSizeChanged()             { Q_EMIT rendererSizeChanged();         }   \
void ClassName::emitBrightnessChanged(qreal v)        { Q_EMIT brightnessChanged(v);          }   \
void ClassName::emitContrastChanged(qreal v)          { Q_EMIT contrastChanged(v);            }   \
void ClassName::emitHueChanged(qreal v)               { Q_EMIT hueChanged(v);                 }   \
void ClassName::emitSaturationChanged(qreal v)        { Q_EMIT saturationChanged(v);          }   \
void ClassName::emitBackgroundColorChanged()          { Q_EMIT backgroundColorChanged();      }

/*!
 * A bridge for VideoOutput(QObject based) and video renderer backend classes
 * Every public setter call it's virtual onSetXXX(...) which has default behavior.
 * While VideoOutput.onSetXXX(...) simply calls backend's setXXX(...) and return whether the result is desired.
 */

class QWidget;
class QWindow;
class QGraphicsItem;

namespace QtAV
{

typedef int VideoRendererId;

extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_OpenGLWindow;

class Filter;
class OpenGLVideo;
class VideoFormat;
class VideoRendererPrivate;

Q_NAMESPACE

enum OutAspectRatioMode
{
    RendererAspectRatio, ///< Use renderer's aspect ratio, i.e. stretch to fit the renderer rect
    VideoAspectRatio,    ///< Use video's aspect ratio and align center in renderer.
    CustomAspectRation   ///< Use the ratio set by setOutAspectRatio(qreal). Mode will be set to this if that function is called
/*
    AspectRatio4_3,
    AspectRatio16_9
*/
};
Q_ENUM_NS(OutAspectRatioMode)

enum Quality
{
     // TODO: deprecated. simpily use int 0~100

     QualityDefault,      ///< good
     QualityBest,
     QualityFastest
};
Q_ENUM_NS(Quality)

class DIGIKAM_EXPORT VideoRenderer : public AVOutput
{
    DPTR_DECLARE_PRIVATE(VideoRenderer)

public:

    // TODO: original video size mode

    // fillmode: keepsize

    template<class C>
    static bool Register(VideoRendererId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    static VideoRenderer* create(VideoRendererId id);
    static VideoRenderer* create(const char* name);

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static VideoRendererId* next(VideoRendererId* id = nullptr);
    static const char* name(VideoRendererId id);
    static VideoRendererId id(const char* name);

public:

    VideoRenderer();
    virtual ~VideoRenderer();

public:

    virtual VideoRendererId id() const = 0;

    bool receive(const VideoFrame& frame);

    /*!
     * \brief setPreferredPixelFormat
     * \param pixfmt
     *  pixfmt will be used if decoded format is not supported by this renderer.
     *  otherwise, use decoded format.
     *  return false if \a pixfmt is not supported and not changed.
     */
    bool setPreferredPixelFormat(VideoFormat::PixelFormat pixfmt);

    /*!
     * \brief preferredPixelFormat
     * \return preferred pixel format. e.g. WidgetRenderer is rgb formats.
     */
    virtual VideoFormat::PixelFormat preferredPixelFormat() const; // virtual?

    /*!
     * \brief forcePreferredPixelFormat
     *  force to use preferredPixelFormat() even if incoming format is supported
     * \param force
     */
    void forcePreferredPixelFormat(bool force = true);
    bool isPreferredPixelFormatForced() const;
    virtual bool isSupported(VideoFormat::PixelFormat pixfmt) const = 0;

    /*!
     * \brief sourceAspectRatio
     * The display aspect ratio of received video frame. 0 for an invalid frame.
     * sourceAspectRatioChanged() (a signal for QObject renderers) will be called
     * if the new frame has a different DAR.
     */
    qreal sourceAspectRatio() const;

    void setOutAspectRatioMode(OutAspectRatioMode mode);
    OutAspectRatioMode outAspectRatioMode() const;

    // If setOutAspectRatio(qreal) is used, then OutAspectRatioMode is CustomAspectRation

    void setOutAspectRatio(qreal ratio);
    qreal outAspectRatio() const;

    void setQuality(Quality q);
    Quality quality() const;

    void resizeRenderer(const QSize& size);
    void resizeRenderer(int width, int height);
    QSize rendererSize() const;
    int rendererWidth() const;
    int rendererHeight() const;

    // geometry size of current video frame. can not use frameSize because qwidget use it

    QSize videoFrameSize() const;

    /*!
     * \brief orientation
     * 0, 90, 180, 270. other values are ignored
     * outAspectRatio() corresponds with orientation == 0.
     * displayed aspect ratio may change if orientation is not 0
     */
    int orientation() const;
    void setOrientation(int value);

    // The video frame rect in renderer youshouldpaint to. e.g.
    // in RendererAspectRatio mode, the rect equals to renderer's

    QRect videoRect() const;

    /*
     * region of interest, ROI
     * invalid rect means the whole source rect
     * null rect is the whole available source rect. e.g. (0, 0, 0, 0) equals whole source rect
     * (20, 30, 0, 0) equals (20, 30, sourceWidth - 20, sourceHeight - 30)
     * if |x| < 1, |y| < 1, |width| < 1, |height| < 1 means the ratio of source rect(normalized value)
     * |width| == 1 or |height| == 1 is a normalized value iff x or y is normalized
     * call realROI() to get the frame rect actually to be render
     * TODO: nagtive width or height means invert direction. is nagtive necessary?
     */
    QRectF regionOfInterest() const;

    // TODO: reset aspect ratio to roi.width/roi/heghit

    void setRegionOfInterest(qreal x, qreal y, qreal width, qreal height);
    void setRegionOfInterest(const QRectF& roi);

    // compute the real ROI

    QRect realROI() const;

    // |w| <= 1, |x| < 1

    QRectF normalizedROI() const;

    // TODO: map normalized

    /*!
     * \brief mapToFrame
     *  map point in VideoRenderer coordinate to VideoFrame, with current ROI
     */
    QPointF mapToFrame(const QPointF& p) const;

    /*!
     * \brief mapFromFrame
     *  map point in VideoFrame coordinate to VideoRenderer, with current ROI
     */
    QPointF mapFromFrame(const QPointF& p) const;

    // to avoid conflicting width QWidget::window()

    virtual QWindow* qwindow()              { return nullptr; }

    /*!
     * \brief widget
     * \return default is 0. A QWidget subclass can return \a this
     */
    virtual QWidget* widget()               { return nullptr; }

    /*!
     * \brief graphicsItem
     * \return default is 0. A QGraphicsItem subclass can return \a this
     */
    virtual QGraphicsItem* graphicsItem()   { return nullptr; }

    /*!
     * \brief brightness, contrast, hue, saturation
     *  values range between -1.0 and 1.0, the default is 0.
     *  value is not changed if does not implementd and onChangingXXX() returns false.
     *  video widget/item will update after if onChangingXXX/setXXX returns true
     * \return \a false if failed to set (may be onChangingXXX not implemented or return false)
     */
    qreal brightness()              const;
    bool setBrightness(qreal brightness);

    qreal contrast()                const;
    bool setContrast(qreal contrast);

    qreal hue()                     const;
    bool setHue(qreal hue);

    qreal saturation()              const;
    bool setSaturation(qreal saturation);

    QColor backgroundColor()        const;
    void setBackgroundColor(const QColor& c);

    /*!
     * \brief opengl
     * Currently you can only use it to set custom shader OpenGLVideo.setUserShader()
     */
    virtual OpenGLVideo* opengl() const
    {
        return nullptr;
    }

protected:

    VideoRenderer(VideoRendererPrivate& d);

    // TODO: batch drawBackground(color, region)=>loop drawBackground(color,rect)

    virtual bool receiveFrame(const VideoFrame& frame) = 0;
    QRegion backgroundRegion() const;
    virtual void drawBackground();

    // draw the current frame using the current paint engine. called by paintEvent()
    // TODO: parameter VideoFrame

    virtual void drawFrame() = 0;    // You MUST reimplement this to display a frame. Other draw functions are not essential
    virtual void handlePaintEvent(); // has default. User don't have to implement it
    virtual void updateUi();         // by default post an UpdateRequest event for window and UpdateLater event for widget to ensure ui update

private: // property change. send signals in subclasses.

    virtual void emitSourceAspectRatioChanged(qreal) = 0;
    virtual void emitOutAspectRatioChanged()         = 0;
    virtual void emitOutAspectRatioModeChanged()     = 0;
    virtual void emitOrientationChanged()            = 0;
    virtual void emitVideoRectChanged()              = 0;
    virtual void emitContentRectChanged()            = 0;
    virtual void emitRegionOfInterestChanged()       = 0;
    virtual void emitVideoFrameSizeChanged()         = 0;
    virtual void emitRendererSizeChanged()           = 0;
    virtual void emitBrightnessChanged(qreal)        = 0;
    virtual void emitContrastChanged(qreal)          = 0;
    virtual void emitHueChanged(qreal)               = 0;
    virtual void emitSaturationChanged(qreal)        = 0;
    virtual void emitBackgroundColorChanged()        = 0;

private: // mainly used by VideoOutput class

    /*!
     * return false if value not changed. default is true
     */
    virtual bool onSetPreferredPixelFormat(VideoFormat::PixelFormat pixfmt);
    virtual bool onForcePreferredPixelFormat(bool force = true);
    virtual void onSetOutAspectRatioMode(OutAspectRatioMode mode);
    virtual void onSetOutAspectRatio(qreal ratio);
    virtual bool onSetQuality(Quality q);
    virtual void onResizeRenderer(int width, int height);
    virtual bool onSetOrientation(int value);
    virtual bool onSetRegionOfInterest(const QRectF& roi);
    virtual QPointF onMapToFrame(const QPointF& p) const;
    virtual QPointF onMapFromFrame(const QPointF& p) const;

    /*!
     * \brief onSetXX
     *  It's called when user call setXXX() with a new value. You should implement how
     * to actually change the value, e.g. change brightness with shader.
     * \return
     *  false: It's default. means not implemented. \a brightness() does not change.
     *  true: Implement this and return true. \a brightness() will change to new value
     */
    virtual bool onSetBrightness(qreal brightness);
    virtual bool onSetContrast(qreal contrast);
    virtual bool onSetHue(qreal hue);
    virtual bool onSetSaturation(qreal saturation);
    virtual void onSetBackgroundColor(const QColor& color);

private:

    template<class C>
    static VideoRenderer* create()
    {
        return new C();
    }

    typedef VideoRenderer*(*VideoRendererCreator)();

    static bool Register(VideoRendererId id, VideoRendererCreator, const char* name);

    friend class VideoOutput;

    // the size of decoded frame. get called in receiveFrame(). internal use only

    void setInSize(const QSize& s);
    void setInSize(int width, int height);
};

} // namespace QtAV

#endif // QTAV_VIDEO_RENDERER_H
