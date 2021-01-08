/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : a class to calculate filter weights for hot pixels tool
 *
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_HOT_PIXELS_WEIGHTS_H
#define DIGIKAM_HOT_PIXELS_WEIGHTS_H

// Qt includes

#include <QList>
#include <QRect>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT HotPixelsWeights
{

public:

    HotPixelsWeights();
    HotPixelsWeights(const HotPixelsWeights& w);
    ~HotPixelsWeights();

    HotPixelsWeights& operator=(const HotPixelsWeights& w);
    bool operator==(const HotPixelsWeights& ws) const;
    double** operator[](int n)                  const;

    unsigned int height()                       const;
    void setHeight(int h);

    unsigned int polynomeOrder()                const;
    void setPolynomeOrder(int order);

    bool twoDim() const;
    void setTwoDim(bool td);

    void setWidth(int w);
    unsigned int width()                        const;

    const QList<QPoint> positions()             const;

    void calculateHotPixelsWeights();

protected:

    int coefficientNumber()                     const;

    double** * weightMatrices()                 const;

private:

    double polyTerm(const size_t i_coeff,
                    const int x,
                    const int y,
                    const int poly_order)       const;

    void   matrixInv(double* const a,
                     const size_t size);

private:

    unsigned int  m_height;
    unsigned int  m_width;
    unsigned int  m_coefficientNumber;
    bool          m_twoDim;
    unsigned int  m_polynomeOrder;
    double** *    m_weightMatrices;     ///< Stores a list of weight matrices
    QList<QPoint> m_positions;
};

} // namespace Digikam

#endif // DIGIKAM_HOT_PIXELS_WEIGHTS_H
