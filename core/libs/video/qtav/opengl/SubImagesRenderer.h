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

#ifndef QTAV_SUB_IMAGES_RENDERER_H
#define QTAV_SUB_IMAGES_RENDERER_H

// Qt includes

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>

// Local includes

#include "SubImage.h"
#include "OpenGLTypes.h"
#include "OpenGLHelper.h"

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
    void render(const SubImageSet& ass, const QRect& target,
                const QMatrix4x4& transform = QMatrix4x4());

    /*!
     * \brief setProjectionMatrixToRect
     * the rect will be viewport
     */
    void setProjectionMatrixToRect(const QRectF& v);

private:

    void uploadTexture(SubImagesGeometry* g);

private:

    SubImagesGeometry*   m_geometry;
    GeometryRenderer*    m_renderer;
    QMatrix4x4           m_mat;
    QRect                m_rect;

    GLuint               m_tex;
    QOpenGLShaderProgram m_program;

private:

    // Disable

    SubImagesRenderer(const SubImagesRenderer&)            = delete;
    SubImagesRenderer& operator=(const SubImagesRenderer&) = delete;
};

} // namespace QtAV

#endif // QTAV_SUB_IMAGES_RENDERER_H
