/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Implementation of KD-Tree for vector space partitioning
 *
 * SPDX-FileCopyrightText: 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "kd_node.h"

namespace Digikam
{

class KDTree
{

public:

    explicit KDTree(int dim);
    ~KDTree();

    /**
     * @return Map of N-nearest neighbors, sorted by distance
     */
    QMap<double, QVector<int> > getClosestNeighbors(const cv::Mat& position,
                                                    float          sqRange,
                                                    float          cosThreshold,
                                                    int            maxNbNeighbors) const;

    /**
     * @brief add new node to KD-Tree
     * @param position : K-dimension vector
     * @param identity : identity of this face vector
     * @return
     */
    KDNode* add(const cv::Mat& position, const int identity);

private:

    // Disable
    KDTree(const KDTree&)            = delete;
    KDTree& operator=(const KDTree&) = delete;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
