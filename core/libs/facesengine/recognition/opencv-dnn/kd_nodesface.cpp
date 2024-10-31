/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Node of KD-Tree for vector space partitioning.
 *
 * SPDX-FileCopyrightText: 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "kd_nodesface.h"

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

KDNodeSFace::KDNodeSFace(const cv::Mat& nodePos,
                         const int      identity,
                         int            splitAxis,
                         int            dimension)
    : KDNodeBase(nodePos,
                 identity,
                 splitAxis,
                 dimension)
{
}

KDNodeSFace::~KDNodeSFace()
{
}

KDNodeBase* KDNodeSFace::createNode(const cv::Mat& nodePos,
                                    const int identity,
                                    int splitAxis,
                                    int dimension)
{
    return new KDNodeSFace(
                           nodePos,
                           identity,
                           splitAxis,
                           dimension
                           //m_net
                          );
}

KDNodeBase::NodeCompareResult KDNodeSFace::nodeCompare(
                                                       const cv::Mat& queryPosition,
                                                       const cv::Mat& currentPosition,
                                                       float sqRange,
                                                       float cosThreshold,
                                                       int nbDimension
                                                      ) const
{
    KDNodeBase::NodeCompareResult result;

    Q_UNUSED(nbDimension);
/*
    const double cosDistance     = m_net->match(queryPosition, currentPosition, cv::FaceRecognizerSF::DisType::FR_COSINE);
    const double norm_l1Distance = m_net->match(queryPosition, currentPosition, cv::FaceRecognizerSF::DisType::FR_NORM_L2);
*/
    double cosDistance     = sum(queryPosition.mul(currentPosition))[0];
    double norm_l1Distance = norm(queryPosition, currentPosition);

    // Recompute cosThreshold for SFace based on passed in sqRange.
    // sqRange is controlled via the UI. cosThreshold is hard coded. Need to fix that later.
    // We use the inverse of the cos result to compare to sqRange
    // and norm_l1 distance compared to recomputed cosThreshold.
    // norm_l1 is almost always < 1 for a good match. We add .1 of sqRange for extra verification
    // to avoid false negatives.

    cosThreshold = 1.0 + (sqRange / 10.0);
/*
    qCDebug(DIGIKAM_FACEDB_LOG) << "Checking cos:" << cosDistance << " norm_l1:" << norm_l1Distance;
    qCDebug(DIGIKAM_FACEDB_LOG) << "params: sqRange" << sqRange << " cosThreshold:" << cosThreshold;
*/
    result.distance1 = 1.0 - cosDistance;
    result.distance2 = norm_l1Distance;

    // NOTE: both Euclidean distance and cosine distance can help to avoid error in similarity prediction.

    result.result = (result.distance1 < sqRange) &&
                    (result.distance2 < cosThreshold);

    return result;
}

} // namespace Digikam
