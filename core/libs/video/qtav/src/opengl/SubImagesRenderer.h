/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QTAV_SUBIMAGESRENDERER_H
#define QTAV_SUBIMAGESRENDERER_H

#include <QMatrix4x4>
#include "SubImage.h"
#include "OpenGLTypes.h"
#include "opengl/OpenGLHelper.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#else
#include <QGLShaderProgram>
#include <QGLShader>
#undef QOpenGLShaderProgram
#undef QOpenGLShader
#define QOpenGLShaderProgram QGLShaderProgram
#define QOpenGLShader QGLShader
#endif

namespace QtAV
{
class SubImagesGeometry;
class GeometryRenderer;
class SubImagesRenderer
{
public:
    SubImagesRenderer();
    ~SubImagesRenderer();
    /*!
     * \brief render
     * \param ass
     * \param target
     * \param transform additional transform, e.g. aspect ratio
     */
    void render(const SubImageSet& ass, const QRect& target, const QMatrix4x4& transform = QMatrix4x4());
    /*!
     * \brief setProjectionMatrixToRect
     * the rect will be viewport
     */
    void setProjectionMatrixToRect(const QRectF& v);

private:
    void uploadTexture(SubImagesGeometry* g);

    SubImagesGeometry *m_geometry;
    GeometryRenderer *m_renderer;
    QMatrix4x4 m_mat;
    QRect m_rect;

    GLuint m_tex;
    QOpenGLShaderProgram m_program;
};
} // namespace QtAV
#endif //QTAV_SUBIMAGESRENDERER_H
