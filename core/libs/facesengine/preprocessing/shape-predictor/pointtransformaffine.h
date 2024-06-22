/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 16/08/2016
 * Description : point transform class and its utilities that models
 *               affine transformations between two sets of 2d-points.
 *
 * SPDX-FileCopyrightText: 2016      by Omar Amin <Omar dot moh dot amin at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <vector>
#include <iostream>

// Local includes

#include "matrixoperations.h"
#include "vectoroperations.h"

namespace Digikam
{

class PointTransformAffine
{
public:

    PointTransformAffine();
    PointTransformAffine(const std::vector<std::vector<float> >& m_,
                         const std::vector<float>& b_);
    explicit PointTransformAffine(const std::vector<std::vector<float> >& m_);

    const std::vector<float> operator() (const std::vector<float>& p) const;

    const std::vector<std::vector<float> >& get_m() const;
    const std::vector<float>& get_b() const;

private:

    std::vector<std::vector<float> > m;
    std::vector<float>               b;
};

// ----------------------------------------------------------------------------------------

PointTransformAffine operator*(const PointTransformAffine& lhs,
                               const PointTransformAffine& rhs);

// ----------------------------------------------------------------------------------------

PointTransformAffine inv(const PointTransformAffine& trans);

// ----------------------------------------------------------------------------------------

template <typename T>
PointTransformAffine findAffineTransform(const std::vector<std::vector<T> >& fromPoints,
                                         const std::vector<std::vector<T> >& toPoints)
{
    std::vector<std::vector<float> > P(3, std::vector<float>(fromPoints.size()));
    std::vector<std::vector<float> > Q(2, std::vector<float>(fromPoints.size()));

    for (unsigned long i = 0 ; i < fromPoints.size() ; ++i)
    {
        P[0][i] = fromPoints[i][0];
        P[1][i] = fromPoints[i][1];
        P[2][i] = 1;

        Q[0][i] = toPoints[i][0];
        Q[1][i] = toPoints[i][1];
    }

    const std::vector<std::vector<float> > m = Q * MatrixOperations::pinv(P);

    return PointTransformAffine(m);
}

// ----------------------------------------------------------------------------------------

PointTransformAffine findSimilarityTransform(const std::vector<std::vector<float> >& fromPoints,
                                             const std::vector<std::vector<float> >& toPoints);

} // namespace Digikam
