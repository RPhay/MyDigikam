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

// C++ includes

#include <vector>

// Qt includes

#include <QVector>
#include <QMap>

// Local include

#include "digikam_opencv.h"
#include "kd_nodebase.h"

// due to sparse data density in the tree, we initially use a vector of nodes to compare the target to the samples
// once we have achieved a suitabe data density we delete the vector (but not the nodes)
// and begin using the tree.
// the next refactor will include replacing the tree with a more appropriate HDLSS classifier

#define KDTREE_MAP_THRESHOLD 500 // size of the vector before we start using the tree

namespace Digikam
{

class KDTreeBase
{

public:

    KDTreeBase(
               int dim,
               int kdtreeThreshold
              );
    virtual ~KDTreeBase();

    /**
     * @return Map of N-nearest neighbors, sorted by distance
     */
    virtual QMap<double, QVector<int> > getClosestNeighbors(const cv::Mat& position,
                                                            float          sqRange,
                                                            int            maxNbNeighbors) const;

    /**
     * @brief add new node to KD-Tree
     * @param position : K-dimension vector
     * @param identity : identity of this face vector
     * @return
     */
    virtual KDNodeBase* add(const cv::Mat& position, const int identity);

private:

    // Disable
    KDTreeBase(const KDTreeBase&)                                                                   = delete;
    KDTreeBase& operator=(const KDTreeBase&)                                                        = delete;

    virtual KDNodeBase* createNode(const cv::Mat& nodePos,
                                   const int identity,
                                   int splitAxis,
                                   int dimension)                                                   = 0;
    virtual float getCosThreshold(float sqRange) const                                              = 0;

private:

    class Private;
    Private* const d                                                                                = nullptr;
};

} // namespace Digikam
