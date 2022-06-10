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

#ifndef QTAV_GLSL_FILTER_H
#define QTAV_GLSL_FILTER_H

// Local includes

#include "QtAV_Global.h"
#include "Filter.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#   undef QOpenGLFramebufferObject
#   define QOpenGLFramebufferObject QGLFramebufferObject
#endif

class QOpenGLFramebufferObject;

namespace QtAV
{

class OpenGLVideo;
class GLSLFilterPrivate;

class DIGIKAM_EXPORT GLSLFilter : public VideoFilter
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(GLSLFilter)
    Q_PROPERTY(QSize outputSize READ outputSize WRITE setOutputSize NOTIFY outputSizeChanged)

public:

    explicit GLSLFilter(QObject* const parent = nullptr);

    bool isSupported(VideoFilterContext::Type ct)   const override
    {
        return (ct == VideoFilterContext::OpenGL);
    }

    /*!
     * \brief opengl
     * Currently you can only use it to set custom shader OpenGLVideo.setUserShader()
     */
    OpenGLVideo* opengl()                           const;
    QOpenGLFramebufferObject* fbo()                 const;

    /*!
     * \brief outputSize
     * Output frame size. FBO uses the same size to render. An empty size means using the input frame size
     * \return
     */
    QSize outputSize()                              const;
    void setOutputSize(const QSize& value);
    void setOutputSize(int width, int height);

Q_SIGNALS:

    void outputSizeChanged(const QSize& size);

protected:

    explicit GLSLFilter(GLSLFilterPrivate& d, QObject* const parent = nullptr);

    /*!
     * \brief process
     * Draw video frame into fbo and apply the user shader from opengl()->userShader();
     * \param frame input frame can be a frame holding host memory data, or any other GPU frame can interop with OpenGL texture (including frames from HW decoders in QtAV).
     * Output frame holds an RGB texture, which can be processed in the next GPU filter, or rendered by OpenGL renderers.
     * When process() is done, FBO before process() is bounded.
     */
    void process(Statistics* statistics, VideoFrame* frame = nullptr) override;
};

} // namespace QtAV

#endif // QTAV_GLSL_FILTER_H
