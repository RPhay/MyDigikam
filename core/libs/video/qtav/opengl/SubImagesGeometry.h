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

#ifndef QTAV_SUB_IMAGES_GEOMETRY_H
#define QTAV_SUB_IMAGES_GEOMETRY_H

// Local includes

#include "Geometry.h"
#include "SubImage.h"

namespace QtAV
{

class SubImagesGeometry : public Geometry
{
public:

    SubImagesGeometry();

    bool setSubImages(const SubImageSet& images);

    /*!
     * \brief generateVertexData
     * \param rect render to. If it's viewport rect, and fit video aspect ratio,
     * ass images created from video frame size needs a scale transform is required when rendering
     * \param useIndecies
     * \param maxWidth
     * \return false if current SubImageSet is invalid
     */
    bool generateVertexData(const QRect& rect, bool useIndecies = false, int maxWidth = -1);

    // available after generateVertexData is called

    int width()
    {
        return m_w;
    }

    int height()
    {
        return m_h;
    }

    int stride()                           const override;

    const QVector<Attribute>& attributes() const override
    {
        return m_attributes;
    }

    const SubImageSet& images()            const
    {
        return m_images;
    }

    const QVector<QRect>& uploadRects()    const
    {
        return m_rects_upload;
    }

private:

    using Geometry::allocate;

    bool               m_normalized;
    int                m_w, m_h;
    QVector<Attribute> m_attributes;
    SubImageSet        m_images;        ///< for texture upload parameters
    QVector<QRect>     m_rects_upload;

private:

    Q_DISABLE_COPY(SubImagesGeometry);
};

} // namespace QtAV

#endif // QTAV_SUB_IMAGES_GEOMETRY_H
