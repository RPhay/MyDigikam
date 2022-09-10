/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : digiKam 8/16 bits image management API.
 *               QImage accessors.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dimg_p.h"

namespace Digikam
{

QImage DImg::copyQImage() const
{
    if (isNull())
    {
        return QImage();
    }

    if (sixteenBit())
    {
        DImg img(*this);
        img.detach();
        img.convertDepth(32);

        return img.copyQImage();
    }

    QImage img(width(), height(), QImage::Format_ARGB32);

    if (img.isNull())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to allocate memory to copy DImg of size" << size() << "to QImage";

        return QImage();
    }

    uchar* sptr = bits();
    uint*  dptr = reinterpret_cast<uint*>(img.bits());

    for (uint i = 0 ; i < width()*height() ; ++i)
    {
        *dptr++ = qRgba(sptr[2], sptr[1], sptr[0], sptr[3]);
        sptr   += 4;
    }

    // NOTE: Qt4 do not provide anymore QImage::setAlphaChannel() because
    // alpha channel is auto-detected during QImage->QPixmap conversion

    return img;
}

QImage DImg::copyQImage(const QRect& rect) const
{
    return (copyQImage(rect.x(), rect.y(), rect.width(), rect.height()));
}

QImage DImg::copyQImage(const QRectF& rel) const
{
    if (isNull() || !rel.isValid())
    {
        return QImage();
    }

    return copyQImage(QRectF(rel.x()      * m_priv->width,
                             rel.y()      * m_priv->height,
                             rel.width()  * m_priv->width,
                             rel.height() * m_priv->height)
                      .toRect());
}

QImage DImg::copyQImage(int x, int y, int w, int h) const
{
    if (isNull())
    {
        return QImage();
    }

    DImg img = copy(x, y, w, h);

    if (img.sixteenBit())
    {
        img.convertDepth(32);
    }

    return img.copyQImage();
}

} // namespace Digikam
