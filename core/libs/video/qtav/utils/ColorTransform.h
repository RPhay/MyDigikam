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

#ifndef QTAV_COLOR_TRANSFORM_H
#define QTAV_COLOR_TRANSFORM_H

// Qt includes

#include <QSharedDataPointer>
#include <QMatrix4x4>

// Local includes

#include "QtAV_Global.h"

// TODO: kernel QGenericMatrix<M,N>

// http://www.graficaobscura.com/matrix/index.html              // krazy:exclude=insecurenet

namespace QtAV
{

/*!
 * \brief The ColorTransform class
 * A convenience class to get color transformation matrix.
 * Implicitly shared.
 */
class ColorTransform
{
public:

    // http://msdn.microsoft.com/en-us/library/dd206750.aspx    // krazy:exclude=insecurenet

    // cs: BT601 or BT709

    static const QMatrix4x4& YUV2RGB(ColorSpace cs);

    ColorTransform();
    ~ColorTransform(); // required by QSharedDataPointer if Private is forward declared

    /*!
     * \brief inputColorSpace
     * if inputColorSpace is different from outputColorSpace, then the result matrix(), matrixRef() and
     * matrixData() will count the transformation between in/out color space.
     * default in/output color space is rgb
     * \param cs
     */
    ColorSpace inputColorSpace()                const;
    void setInputColorSpace(ColorSpace cs);

    ColorSpace outputColorSpace()               const;
    void setOutputColorSpace(ColorSpace cs);

    /**
     * Currently assume input is yuv, output is rgb
     */
    void setInputColorRange(ColorRange value);
    ColorRange inputColorRange()                const;

    void setOutputColorRange(ColorRange value);
    ColorRange outputColorRange()               const;

    /*!
     * \brief matrix
     * \return result matrix to transform from inputColorSpace
     * to outputColorSpace with given brightness,
     * contrast, saturation and hue
     */
    QMatrix4x4 matrix()                         const;
    const QMatrix4x4& matrixRef()               const;

    /*!
     * Get the matrix in column-major order. Used by OpenGL
     */
    template<typename T> void matrixData(T* M)  const
    {
        const QMatrix4x4& m = matrixRef();
        M[0] = m(0,0), M[4] = m(0,1), M[8]  = m(0,2), M[12] = m(0,3),
        M[1] = m(1,0), M[5] = m(1,1), M[9]  = m(1,2), M[13] = m(1,3),
        M[2] = m(2,0), M[6] = m(2,1), M[10] = m(2,2), M[14] = m(2,3),
        M[3] = m(3,0), M[7] = m(3,1), M[11] = m(3,2), M[15] = m(3,3);
    }

    /*!
     * \brief reset
     * only set in-space transform to identity.
     * other parameters such as in/out color space does not change
     */
    void reset();

    // -1~1

    void setBrightness(qreal brightness);
    qreal brightness()                          const;

    // -1~1

    void setHue(qreal hue);
    qreal hue()                                 const;

    // -1~1

    void setContrast(qreal contrast);
    qreal contrast()                            const;

    // -1~1

    void setSaturation(qreal saturation);
    qreal saturation()                          const;

    void setChannelDepthScale(qreal value, bool scaleAlpha = false);

private:

    class Private;
    QSharedDataPointer<ColorTransform::Private> d;
};

} // namespace QtAV

#endif // QTAV_COLOR_TRANSFORM_H
