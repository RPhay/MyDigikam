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

#include "GLSLFilter.h"

// Qt includes

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#   include <QOpenGLFramebufferObject>
#else
#   include <QGLFramebufferObject>
#endif

// Local includes

#include "Filter_p.h"
#include "VideoFrame.h"
#include "OpenGLHelper.h"
#include "SurfaceInterop.h"
#include "OpenGLVideo.h"
#include "digikam_debug.h"

namespace QtAV
{
class Q_DECL_HIDDEN GLSLFilterPrivate : public VideoFilterPrivate
{
public:

    GLSLFilterPrivate()
        : VideoFilterPrivate(),
          fbo               (nullptr)
    {
    }

    QOpenGLFramebufferObject* fbo;
    QSize                     size;
    OpenGLVideo               glv;
};

GLSLFilter::GLSLFilter(QObject* const parent)
    : VideoFilter(*new GLSLFilterPrivate(), parent)
{
}

GLSLFilter::GLSLFilter(GLSLFilterPrivate& d, QObject* const parent)
    : VideoFilter(d, parent)
{
}

OpenGLVideo* GLSLFilter::opengl() const
{
    return const_cast<OpenGLVideo*>(&d_func().glv);
}

QOpenGLFramebufferObject* GLSLFilter::fbo() const
{
    return d_func().fbo;
}

QSize GLSLFilter::outputSize() const
{
    return d_func().size;
}

void GLSLFilter::setOutputSize(const QSize& value)
{
    DPTR_D(GLSLFilter);

    if (d.size == value)
        return;

    d.size = value;

    Q_EMIT outputSizeChanged(value);
}

void GLSLFilter::setOutputSize(int width, int height)
{
    setOutputSize(QSize(width, height));
}

void GLSLFilter::process(Statistics* statistics, VideoFrame* frame)
{
    Q_UNUSED(statistics);

    if (!QOpenGLContext::currentContext())
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "No current gl context for glsl filter: " << this;

        return;
    }

    DPTR_D(GLSLFilter);

    if (!frame || !*frame)
        return;

    GLint currentFbo = 0;
    DYGL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo));

    // now use the frame size

    if (d.fbo)
    {
        if (d.size.isEmpty())
        {
            if (d.fbo->size() != frame->size())
            {
                delete d.fbo;
                d.fbo = nullptr;
            }
        }
        else
        {
            if (d.fbo->size() != d.size)
            {
                delete d.fbo;
                d.fbo = nullptr;
            }
        }
    }

    if (!d.fbo)
    {
        d.fbo                     = new QOpenGLFramebufferObject(outputSize().isEmpty() ? frame->size() : outputSize(), GL_TEXTURE_2D); //TODO: prefer 16bit rgb
        QOpenGLContext* const ctx = const_cast<QOpenGLContext*>(QOpenGLContext::currentContext()); // qt4 returns const
        d.glv.setOpenGLContext(ctx);
        d.glv.setProjectionMatrixToRect(QRectF(0, 0, d.fbo->width(), d.fbo->height()));

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("new fbo texture: %d %dx%d",
                d.fbo->texture(), d.fbo->width(), d.fbo->height());
    }

    d.fbo->bind();
    DYGL(glViewport(0, 0, d.fbo->width(), d.fbo->height()));
    d.glv.setCurrentFrame(*frame);
    QMatrix4x4 mat;                     // flip vertical
    mat.scale(1, -1);
    d.glv.render(QRectF(), QRectF(), mat);
    gl().BindFramebuffer(GL_FRAMEBUFFER, (GLuint)currentFbo);
    VideoFormat fmt(VideoFormat::Format_RGB32);
    VideoFrame f(d.fbo->width(), d.fbo->height(), fmt);
    f.setBytesPerLine(d.fbo->width()*fmt.bytesPerPixel(), 0);

    // set interop;

    class Q_DECL_HIDDEN GLTextureInterop : public VideoSurfaceInterop
    {
    public:

        explicit GLTextureInterop(GLuint id)
            : tex(id)
        {
        }

        void* map(SurfaceType, const VideoFormat&, void* const handle, int plane)
        {
            Q_UNUSED(plane);

            GLuint* t = reinterpret_cast<GLuint*>(handle);
            *t        = tex;

            return t;
        }

    private:

        GLuint tex;

    private:

        Q_DISABLE_COPY(GLTextureInterop);
    };

    GLTextureInterop* const interop = new GLTextureInterop(d.fbo->texture());
    f.setMetaData(QLatin1String("surface_interop"), QVariant::fromValue(VideoSurfaceInteropPtr((interop))));
    *frame                          = f;
}

} // namespace QtAV
