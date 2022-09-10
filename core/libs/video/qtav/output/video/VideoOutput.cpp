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

#include "VideoOutput.h"

// Qt includes

#include <QLibrary>
#include <QString>
#include <QWidget>
#include <QResizeEvent>

// Local includes

#include "QtAV_Version.h"
#include "VideoRenderer_p.h"
#include "digikam_debug.h"

/*!
 * onSetXXX(...): impl->onSetXXX(...); set value as impl; return ;
 */

namespace QtAV
{

class Q_DECL_HIDDEN VideoOutputPrivate : public VideoRendererPrivate
{
public:

    VideoOutputPrivate(VideoRendererId rendererId, bool force)
    {
        if (!VideoRenderer::id("Widget"))
        {
            // TODO: dso version check?

#if defined(Q_OS_DARWIN)

            // no dylib check

            avwidgets.setFileName(QString::fromLatin1("QtAVWidgets.framework/QtAVWidgets"));

#elif defined(Q_OS_WIN)

            avwidgets.setFileName(QString::fromLatin1("QtAVWidgets")

#   ifndef QT_NO_DEBUG

            .append(QLatin1String("d"))

#   endif

            .append(QString::number(QTAV_VERSION_MAJOR(QtAV_Version()))));

#else

            avwidgets.setFileNameAndVersion(QLatin1String("QtAVWidgets"),
                                            QTAV_VERSION_MAJOR(QtAV_Version()));

#endif

            qCDebug(DIGIKAM_QTAV_LOG)
                << "Loading QtAVWidgets module: "
                << avwidgets.fileName();

            if (!avwidgets.load())
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("Failed to load QtAVWidgets module");
            }
        }

        impl = VideoRenderer::create(rendererId);

        if (!impl && !force)
        {
            VideoRendererId* vid = nullptr;

            while ((vid = VideoRenderer::next(vid)))
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("next id: %d, name: %s",
                        *vid, VideoRenderer::name(*vid));

                if (impl)
                {
                    delete impl;
                    impl = nullptr;
                }

                impl = VideoRenderer::create(*vid);

                if (impl && impl->isAvailable() && impl->widget())
                    break;
            }
        }

        available = !!impl;

        if (!available)
            return;

        // set members as impl's that may be already set in ctor

        filters                 = impl->filters();
        renderer_width          = impl->rendererWidth();
        renderer_height         = impl->rendererHeight();
        src_width               = impl->videoFrameSize().width();
        src_height              = impl->videoFrameSize().height();
        source_aspect_ratio     = qreal(src_width) / qreal(src_height);
        out_aspect_ratio_mode   = impl->outAspectRatioMode();
        out_aspect_ratio        = impl->outAspectRatio();
        quality                 = impl->quality();
        out_rect                = impl->videoRect();
        roi                     = impl->regionOfInterest();
        preferred_format        = impl->preferredPixelFormat();
        force_preferred         = impl->isPreferredPixelFormatForced();
        brightness              = impl->brightness();
        contrast                = impl->contrast();
        hue                     = impl->hue();
        saturation              = impl->saturation();
    }

    ~VideoOutputPrivate()
    {
        if (impl)
        {
            QObject* const obj = reinterpret_cast<QObject*>(impl->widget());

            if (obj && !obj->parent())
                obj->deleteLater();

            impl = nullptr;
        }
    }

public:

    VideoRenderer* impl = nullptr;
    QLibrary       avwidgets;
};

VideoOutput::VideoOutput(QObject* const parent)
    : QObject      (parent),
      VideoRenderer(*new VideoOutputPrivate(0, false))
{
    if (d_func().impl && d_func().impl->widget())
    {
        dynamic_cast<QObject*>(d_func().impl->widget())->installEventFilter(this);
    }
}

VideoOutput::VideoOutput(VideoRendererId rendererId, QObject* const parent)
    : QObject      (parent),
      VideoRenderer(*new VideoOutputPrivate(rendererId, true))
{
    if (d_func().impl && d_func().impl->widget())
    {
        dynamic_cast<QObject*>(d_func().impl->widget())->installEventFilter(this);
    }
}

VideoOutput::~VideoOutput()
{
    if (d_func().impl && d_func().impl->widget())
    {
        dynamic_cast<QObject*>(d_func().impl->widget())->removeEventFilter(this);
    }
}

VideoRendererId VideoOutput::id() const
{
    if (!isAvailable())
        return 0;

    return d_func().impl->id();
}

bool VideoOutput::onSetPreferredPixelFormat(VideoFormat::PixelFormat pixfmt)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    d.impl->setPreferredPixelFormat(pixfmt);

    return (pixfmt == d.impl->preferredPixelFormat());

}

VideoFormat::PixelFormat VideoOutput::preferredPixelFormat() const
{
    if (!isAvailable())
        return VideoFormat::Format_Invalid;

    return d_func().impl->preferredPixelFormat();
}

bool VideoOutput::isSupported(VideoFormat::PixelFormat pixfmt) const
{
    if (!isAvailable())
        return false;

    return d_func().impl->isSupported(pixfmt);
}

QWindow* VideoOutput::qwindow()
{
    if (!isAvailable())
        return nullptr;

    return d_func().impl->qwindow();
}

QWidget* VideoOutput::widget()
{
    if (!isAvailable())
        return nullptr;

    return d_func().impl->widget();
}

QGraphicsItem* VideoOutput::graphicsItem()
{
    if (!isAvailable())
        return nullptr;

    return d_func().impl->graphicsItem();
}

OpenGLVideo* VideoOutput::opengl() const
{
    if (!isAvailable())
        return nullptr;

    return d_func().impl->opengl();
}

bool VideoOutput::eventFilter(QObject* obj, QEvent* event)
{
    DPTR_D(VideoOutput);

    if (!d.impl || (dynamic_cast<QObject*>(d.impl->widget()) != obj))
        return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::Resize)
    {
        QResizeEvent* const re = static_cast<QResizeEvent*>(event);
        resizeRenderer(re->size());
    }

    return QObject::eventFilter(obj, event);
}

bool VideoOutput::receiveFrame(const VideoFrame& frame)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    d.impl->d_func().source_aspect_ratio = d.source_aspect_ratio;
    d.impl->setInSize(frame.size());
    QMutexLocker locker(&d.impl->d_func().img_mutex);

    Q_UNUSED(locker); // TODO: double buffer for display/dec frame to avoid mutex

    return d.impl->receiveFrame(frame);
}

void VideoOutput::drawBackground()
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->drawBackground();
}

void VideoOutput::drawFrame()
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->drawFrame();
}

void VideoOutput::handlePaintEvent()
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->handlePaintEvent();
}

bool VideoOutput::onForcePreferredPixelFormat(bool force)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    d.impl->forcePreferredPixelFormat(force);

    return (d.impl->isPreferredPixelFormatForced() == force);
}

void VideoOutput::onSetOutAspectRatioMode(OutAspectRatioMode mode)
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->setOutAspectRatioMode(mode);
}

void VideoOutput::onSetOutAspectRatio(qreal ratio)
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->setOutAspectRatio(ratio);
}

bool VideoOutput::onSetQuality(Quality q)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    d.impl->setQuality(q);

    return (d.impl->quality() == q);
}

bool VideoOutput::onSetOrientation(int value)
{
    if (!isAvailable())
        return false;

    value = (value + 360) % 360;

    DPTR_D(VideoOutput);
    d.impl->setOrientation(value);

    if (d.impl->orientation() != value)
    {
        return false;
    }

    return true;
}

void VideoOutput::onResizeRenderer(int width, int height)
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->resizeRenderer(width, height);
}

bool VideoOutput::onSetRegionOfInterest(const QRectF& roi)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    d.impl->setRegionOfInterest(roi);

    return true;
}

QPointF VideoOutput::onMapToFrame(const QPointF& p) const
{
    if (!isAvailable())
        return QPointF();

    DPTR_D(const VideoOutput);

    return d.impl->onMapToFrame(p);
}

QPointF VideoOutput::onMapFromFrame(const QPointF& p) const
{
    if (!isAvailable())
        return QPointF();

    DPTR_D(const VideoOutput);

    return d.impl->onMapFromFrame(p);
}

bool VideoOutput::onSetBrightness(qreal brightness)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    // not call onSetXXX here, otherwise states in impl will not change

    d.impl->setBrightness(brightness);

    if (brightness != d.impl->brightness())
    {
        return false;
    }

    return true;
}

bool VideoOutput::onSetContrast(qreal contrast)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    // not call onSetXXX here, otherwise states in impl will not change

    d.impl->setContrast(contrast);

    if (contrast != d.impl->contrast())
    {
        return false;
    }

    return true;
}

bool VideoOutput::onSetHue(qreal hue)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    // not call onSetXXX here, otherwise states in impl will not change

    d.impl->setHue(hue);

    if (hue != d.impl->hue())
    {
        return false;
    }

    return true;
}

bool VideoOutput::onSetSaturation(qreal saturation)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    // not call onSetXXX here, otherwise states in impl will not change

    d.impl->setSaturation(saturation);

    if (saturation != d.impl->saturation())
    {
        return false;
    }

    return true;
}

void VideoOutput::onSetBackgroundColor(const QColor& color)
{
    if (!isAvailable())
        return;

    d_func().impl->setBackgroundColor(color);
}

void VideoOutput::setStatistics(Statistics* statistics)
{
    if (!isAvailable())
        return;

    DPTR_D(VideoOutput);

    d.impl->setStatistics(statistics);

    // only used internally for AVOutput
    // d.statistics =
}

bool VideoOutput::onInstallFilter(Filter* filter, int index)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    bool ret  = d.impl->onInstallFilter(filter, index);
    d.filters = d.impl->filters();

    return ret;
}

bool VideoOutput::onUninstallFilter(Filter* filter)
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    bool ret = d.impl->onUninstallFilter(filter);

    // only used internally for AVOutput
    // d.pending_uninstall_filters =

    return ret;
}

bool VideoOutput::onHanlePendingTasks()
{
    if (!isAvailable())
        return false;

    DPTR_D(VideoOutput);

    if (!d.impl->onHanlePendingTasks())
        return false;

    d.filters = d.impl->filters();

    return true;
}

IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(VideoOutput)

} // namespace QtAV
