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

#ifndef QTAV_GEOMETRY_H
#define QTAV_GEOMETRY_H

// Qt includes

#include <QRectF>
#include <QVector>

// Local includes

#include "QtAV_Global.h"

// TODO: generate vertex/fragment shader code from Geometry.attributes()

namespace QtAV
{

enum DataType
{
    // equals to GL_BYTE etc.

    TypeS8  = 0x1400, ///< S8
    TypeU8  = 0x1401, ///< U8
    TypeS16 = 0x1402, ///< S16
    TypeU16 = 0x1403, ///< U16
    TypeS32 = 0x1404, ///< S32
    TypeU32 = 0x1405, ///< U32
    TypeF32 = 0x1406  ///< F32
};

class DIGIKAM_EXPORT Attribute
{
public:

    explicit Attribute(DataType type = TypeF32, int tupleSize = 0,
                       int offset = 0, bool normalize = false);
    explicit Attribute(const QByteArray& name, DataType type = TypeF32,
                       int tupleSize = 0, int offset = 0, bool normalize = false);

    QByteArray name() const { return m_name;      }
    DataType type()   const { return m_type;      }
    int tupleSize()   const { return m_tupleSize; }
    int offset()      const { return m_offset;    }
    bool normalize()  const { return m_normalize; }

    bool operator==(const Attribute& other) const
    {
        return ((tupleSize() == other.tupleSize()) &&
                (offset()    == other.offset())    &&
                (type()      == other.type())      &&
                (normalize() == other.normalize()));
    }

private:

    bool        m_normalize;
    DataType    m_type;
    int         m_tupleSize, m_offset;
    QByteArray  m_name;
};

#ifndef QT_NO_DEBUG_STREAM

DIGIKAM_EXPORT QDebug operator<<(QDebug debug, const Attribute& a);

#endif

/*!
 * \brief The Geometry class
 * \code
 * Q_FOREACH (const Attribute& a, g->attributes()) {
 *     program()->setAttributeBuffer(a.name(), a.type(), a.offset(), a.tupleSize(), g->stride());
 * }
 * \endcode
 */
class DIGIKAM_EXPORT Geometry
{
public:

    /**
     * Strip or Triangles is preferred by ANGLE. The values are equal to opengl
     */
    enum Primitive
    {
        Triangles     = 0x0004,
        TriangleStrip = 0x0005, ///< default
        TriangleFan   = 0x0006  ///< Not recommended
    };

    explicit Geometry(int vertexCount = 0, int indexCount = 0, DataType indexType = TypeU16);
    virtual ~Geometry()
    {
    }

    Primitive primitive() const         { return m_primitive;                                     }
    void setPrimitive(Primitive value)  { m_primitive = value;                                    }
    int vertexCount() const             { return m_vcount;                                        }
    void setVertexCount(int value)      { m_vcount = value;                                       } // TODO: remove, or allocate data here

    // TODO: setStride and no virtual

    virtual int stride() const = 0;

    // TODO: add/set/remove attributes()

    virtual const QVector<Attribute>& attributes() const = 0;

    void* vertexData()                  { return m_vdata.data();                                  }
    const void* vertexData() const      { return m_vdata.constData();                             }
    const void* constVertexData() const { return m_vdata.constData();                             }

    void dumpVertexData();

    void* indexData()                   { return (m_icount > 0) ? m_idata.data()      : nullptr;  }
    const void* indexData() const       { return (m_icount > 0) ? m_idata.constData() : nullptr;  }
    const void* constIndexData() const  { return (m_icount > 0) ? m_idata.constData() : nullptr;  }
    int indexCount() const              { return m_icount;                                        }

    int indexDataSize() const;

    // GL_UNSIGNED_BYTE/SHORT/INT

    DataType indexType() const          { return m_itype;                                         }
    void setIndexType(DataType value)   { m_itype = value;                                        }

    void setIndexValue(int index, int value);
    void setIndexValue(int index, int v1, int v2, int v3); // a triangle
    void dumpIndexData();

    /*!
     * \brief allocate
     * Call allocate() when all parameters are set. vertexData() may change
     */
    void allocate(int nbVertex, int nbIndex = 0);

    /*!
     * \brief compare
     * Compare each attribute and stride that used in glVertexAttribPointer
     * \return true if equal
     */
    bool compare(const Geometry* other) const;

protected:

    Primitive   m_primitive;
    DataType    m_itype;
    int         m_vcount;
    int         m_icount;
    QByteArray  m_vdata;
    QByteArray  m_idata;

private:

    Q_DISABLE_COPY(Geometry);
};

// ----------------------------------------------------------------------

class DIGIKAM_EXPORT TexturedGeometry : public Geometry
{
public:

    TexturedGeometry();

    /*!
     * \brief setTextureCount
     * sometimes we needs more than 1 texture coordinates,
     * for example we have to set rectangle texture
     * coordinates for each plane.
     */
    void setTextureCount(int value);
    int textureCount() const;

    void setRect(const QRectF& r, const QRectF& tr, int texIndex = 0);
    void setGeometryRect(const QRectF& r);
    void setTextureRect(const QRectF& tr, int texIndex = 0);

    int stride() const override { return (2 * sizeof(float) * (textureCount() + 1)); }

    const QVector<Attribute>& attributes() const override;
    virtual void create();

private:

    void setPoint(int index, const QPointF& p, const QPointF& tp, int texIndex = 0);
    void setGeometryPoint(int index, const QPointF& p);
    void setTexturePoint(int index, const QPointF& tp, int texIndex = 0);

protected:

    int                 nb_tex;
    QRectF              geo_rect;
    QVector<QRectF>     texRect;
    QVector<Attribute>  a;

private:

    Q_DISABLE_COPY(TexturedGeometry);
};

// -----------------------------------------------------------------------------------

class DIGIKAM_EXPORT Sphere : public TexturedGeometry
{
public:

    Sphere();

    void setResolution(int w, int h); // >= 2x2
    void setRadius(float value);
    float radius() const;
    void create()        override;

    int stride()   const override
    {
        return (3 * sizeof(float) + 2 * sizeof(float) * textureCount());
    }

protected:

    using Geometry::setPrimitive;

private:

    int   ru;
    int   rv;
    float r;

private:

    Q_DISABLE_COPY(Sphere);
};

} // namespace QtAV

#endif // QTAV_GEOMETRY_H
