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

#ifndef QTAV_GEOMETRY_RENDERER_H
#define QTAV_GEOMETRY_RENDERER_H

#include "Geometry.h"

// Qt includes

#define QT_VAO (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))

#if QT_VAO
#   include <QOpenGLVertexArrayObject>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#   include <QOpenGLBuffer>
#else
#   include <QGLBuffer>
typedef QGLBuffer QOpenGLBuffer;
#endif

QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

namespace QtAV
{

/*!
 * \brief The GeometryRenderer class
 * TODO: support multiple geometry with same primitive (glPrimitiveRestartIndex, glDrawArraysInstanced, glDrawArraysInstanced, glMultiDrawArrays...)
 */
class Q_AV_EXPORT GeometryRenderer
{
public:

    // rendering features. Use all possible features as the default.

    static const int kVBO       = 0x01;
    static const int kIBO       = 0x02;
    static const int kVAO       = 0x04;
    static const int kMapBuffer = 1<<16;

    // TODO: VAB, VBUM etc.

    GeometryRenderer();
    virtual ~GeometryRenderer() {}

    // call updateBuffer internally in bindBuffer if feature is changed

    void setFeature(int f, bool on);
    void setFeatures(int value);
    int features() const;
    int actualFeatures() const;
    bool testFeatures(int value) const;

    /*!
     * \brief updateGeometry
     * Update geometry buffer. Rebind VBO, IBO to VAO if geometry attributes is changed.
     * Assume attributes are bound in the order 0, 1, 2,....
     * Make sure the old geometry is not destroyed before this call because it will be used to compare with the new \l geo
     * \param geo null: release vao/vbo
     */
    void updateGeometry(Geometry* geo = NULL);
    virtual void render();

protected:

    void bindBuffers();
    void unbindBuffers();

private:

    Geometry*       g;
    int             features_;
    int             vbo_size, ibo_size; // QOpenGLBuffer.size() may get error 0x501
    QOpenGLBuffer   vbo;                // VertexBuffer

#if QT_VAO

    QOpenGLVertexArrayObject vao;

#endif

    QOpenGLBuffer ibo;

    // geometry characteristic

    int                stride;
    QVector<Attribute> attrib;
};

} // namespace QtAV

#endif // QTAV_GEOMETRY_RENDERER_H
