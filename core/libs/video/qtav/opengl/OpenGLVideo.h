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

#ifndef QTAV_OPENGL_VIDEO_H
#define QTAV_OPENGL_VIDEO_H

// Qt includes

#ifndef QT_NO_OPENGL
#   include <QHash>
#   include <QMatrix4x4>
#   include <QObject>
#   include "QtAV_Global.h"
#   include "VideoFormat.h"
#   include <QOpenGLContext>

class QColor;

namespace QtAV
{

class VideoFrame;
class VideoShader;
class OpenGLVideoPrivate;

/*!
 * \brief The OpenGLVideo class
 * high level api for renderering a video frame. use VideoShader, VideoMaterial and ShaderManager internally.
 * By default, VBO is used. Set environment var QTAV_NO_VBO=1 or 0 to disable/enable VBO.
 * VAO will be enabled if supported. Disabling VAO is the same as VBO.
 */
class DIGIKAM_EXPORT OpenGLVideo : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(OpenGLVideo)

public:

    enum MeshType
    {
        RectMesh,
        SphereMesh
    };

public:

    static bool isSupported(VideoFormat::PixelFormat pixfmt);
    OpenGLVideo();

    /*!
     * \brief setOpenGLContext
     * a context must be set before renderering.
     * \param ctx
     * 0: current context in OpenGL is done. shaders will be released.
     * QOpenGLContext is QObject in Qt5, and gl resources here will
     * be released automatically if context is destroyed.
     * But you have to call setOpenGLContext(0) for Qt4 explicitly in the old context.
     * Viewport is also set here using context surface/paintDevice size and devicePixelRatio.
     * devicePixelRatio may be wrong for multi-screen with 5.0<qt<5.5,
     * so you should call setProjectionMatrixToRect later in this case
     */
    void setOpenGLContext(QOpenGLContext* ctx);
    QOpenGLContext* openGLContext();

    void setCurrentFrame(const VideoFrame& frame);
    void fill(const QColor& color);

    /*!
     * \brief render
     * all are in Qt's coordinate
     * \param target: the rect renderering to. in Qt's coordinate.
     * not normalized here but in shader. // TODO: normalized check?
     * invalid value (default) means renderering to the whole viewport
     * \param roi: normalized rect of texture to renderer.
     * \param transform: additinal transformation.
     */
    void render(const QRectF& target = QRectF(),
                const QRectF& roi = QRectF(), const QMatrix4x4& transform = QMatrix4x4());

    /*!
     * \brief setProjectionMatrixToRect
     * the rect will be viewport
     */
    void setProjectionMatrixToRect(const QRectF& v);

    void setViewport(const QRectF& r);

    void setBrightness(qreal value);
    void setContrast(qreal value);
    void setHue(qreal value);
    void setSaturation(qreal value);

    void setUserShader(VideoShader* shader);
    VideoShader* userShader() const;

    void setMeshType(MeshType value);
    MeshType meshType() const;

Q_SIGNALS:

    void beforeRendering();

    /*!
     * \brief afterRendering
     * Emitted when video frame is rendered.
     * With DirectConnection, it can be used to draw GL on top of video,
     * or to do screen scraping of the current frame buffer.
     */
    void afterRendering();

protected:

    DPTR_DECLARE(OpenGLVideo)

private Q_SLOTS:

    /**
     * used by Qt5 whose QOpenGLContext is QObject and we can call
     * this when context is about to destroy.
     * shader manager and material will be reset
     */
    void resetGL();
    void updateViewport();

private:

    OpenGLVideo(QObject*);
};

} // namespace QtAV

#endif // QT_NO_OPENGL

#endif // QTAV_OPENGLVIDEO_H
