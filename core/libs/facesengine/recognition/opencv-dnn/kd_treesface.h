/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Implementation of KD-Tree for vector space partitioning.
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
#include "kd_nodesface.h"

namespace Digikam
{

class KDTreeSFace: public KDTreeBase
{

public:

    explicit KDTreeSFace(int dim,
                         int threshold = KDTREE_MAP_THRESHOLD);
    ~KDTreeSFace() override;

private:

    virtual KDNodeBase* createNode(const cv::Mat& nodePos,
                                   const int identity,
                                   int splitAxis,
                                   int dimension)      override;

    virtual float getCosThreshold(float sqRange) const override
    {
        return 1.0 + (sqRange / 10.0);
    }

private:

    // Disable
    KDTreeSFace(const KDTreeSFace&)            = delete;
    KDTreeSFace& operator=(const KDTreeSFace&) = delete;
};

} // namespace Digikam
