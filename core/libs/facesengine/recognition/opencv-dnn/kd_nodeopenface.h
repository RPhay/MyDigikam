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

#pragma once

// C++ includes

#include <vector>

// Qt includes

#include <QVector>
#include <QMap>

// Local include

#include "digikam_opencv.h"
#include "kd_nodebase.h"

namespace Digikam
{

class KDNodeOpenFace: public KDNodeBase
{
public:

    explicit KDNodeOpenFace(const cv::Mat& nodePos,
                            const int      identity,
                            int            splitAxis,
                            int            dimension);
    virtual ~KDNodeOpenFace();

protected:

    KDNodeBase* createNode(const cv::Mat& nodePos,
                           const int identity,
                           int splitAxis,
                           int dimension) override;

    KDNodeBase::NodeCompareResult nodeCompare(
                                              const cv::Mat& queryPosition,
                                              const cv::Mat& currentPosition,
                                              float sqRange,
                                              float cosThreshold,
                                              int nbDimension
                                             ) const override;

private:

    // Disable
    KDNodeOpenFace(const KDNodeOpenFace&)            = delete;
    KDNodeOpenFace& operator=(const KDNodeOpenFace&) = delete;
};

} // namespace Digikam
