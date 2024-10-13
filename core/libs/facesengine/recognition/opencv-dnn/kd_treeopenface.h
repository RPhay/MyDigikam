/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Implementation of KD-Tree for vector space partitioning
 *
 * SPDX-FileCopyrightText: 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "kd_treebase.h"

namespace Digikam
{

class KDTreeOpenFace: public KDTreeBase
{

public:

    explicit KDTreeOpenFace(
                            int dim,
                            int threshold = KDTREE_MAP_THRESHOLD
                           );
    ~KDTreeOpenFace();

private:

    // Disable
    KDTreeOpenFace(const KDTreeOpenFace&)            = delete;
    KDTreeOpenFace& operator=(const KDTreeOpenFace&) = delete;

private:

    virtual KDNodeBase* createNode(const cv::Mat& nodePos,
                                   const int identity,
                                   int splitAxis,
                                   int dimension) override;

    virtual float getCosThreshold(float sqRange) const override
    {
        Q_UNUSED(sqRange);

        return 0.8;
    }
};

} // namespace Digikam
