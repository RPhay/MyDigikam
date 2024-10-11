/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Node of KD-Tree for vector space partitioning
 *
 * SPDX-FileCopyrightText: 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "kd_nodeopenface.h"

// C++ include

#include <cfloat>
#include <cstdio>
#include <iterator>

// Qt include

#include <QtMath>
#include <QDebug>

// Local includes

#include "digikam_debug.h"
#include "dnnfaceextractorbase.h"

namespace Digikam
{

KDNodeOpenFace::KDNodeOpenFace(const cv::Mat& nodePos,
                               const int identity,
                               int splitAxis,
                               int dimension)
                : KDNodeBase(nodePos,
                             identity,
                             splitAxis,
                             dimension)
{
}

KDNodeOpenFace::~KDNodeOpenFace()
{
}

KDNodeBase* KDNodeOpenFace::createNode(const cv::Mat& nodePos,
                                       const int identity,
                                       int splitAxis,
                                       int dimension)
{
    return new KDNodeOpenFace(
                              nodePos,
                              identity,
                              splitAxis,
                              dimension
                             );
}


KDNodeBase::NodeCompareResult KDNodeOpenFace::nodeCompare(
                                                          const cv::Mat& queryPosition,
                                                          const cv::Mat& currentPosition,
                                                          float sqRange,
                                                          float cosThreshold,
                                                          int nbDimension
                                                         ) const
{
    KDNodeBase::NodeCompareResult result;

    const double sqrDistanceToCurrentNode = sqrDistance(queryPosition.ptr<float>(), currentPosition.ptr<float>(), nbDimension);

    result.distance1                      = sqrDistanceToCurrentNode;
    result.distance2                      = cosDistance(queryPosition.ptr<float>(), currentPosition.ptr<float>(), nbDimension);

    // NOTE: both Euclidean distance and cosine distance can help to avoid error in similarity prediction

    result.result = (result.distance1 < sqRange) &&
                    (result.distance2 > cosThreshold);

    return result;
}

} // namespace Digikam
