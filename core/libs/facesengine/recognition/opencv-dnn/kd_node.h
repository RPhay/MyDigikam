/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Node of KD-Tree for vector space partitioning
 *
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef KD_NODE_H
#define KD_NODE_H

#include <QVector>
#include <QMap>

#include <vector>

// digiKam include
#include "digikam_opencv.h"

namespace Digikam
{

class KDNode
{
public:

    explicit KDNode(const cv::Mat& nodePos,
                    const int      identity,
                    int            splitAxis,
                    int            dimension);
    ~KDNode();

public:

    /**
     * Insert a new node to the sub-tree
     */
    KDNode* insert(const cv::Mat& nodePos, const int identity);

    /**
     * Return position vector of a node
     */
    cv::Mat getPosition() const;

    /**
     * Return a list of closest neighbors, limited by maxNbNeighbors and sqRange
     */
    double getClosestNeighbors(QMap<double, QVector<int> >& neighborList,
                               const cv::Mat&               position,
                               float                        sqRange,
                               int                          maxNbNeighbors) const;
    /**
     * Return identity of the node
     */
    int getIdentity();

    /**
     * Set database entry ID of the node
     */
    void setNodeId(int id);

private:

    void updateRange(const cv::Mat&);

    KDNode* findParent(const cv::Mat& nodePos);

private:

    class Private;
    Private* d;

    // Hidden copy constructor and assignment operator
    KDNode(const KDNode&);
    KDNode& operator=(const KDNode&);
};

} // namespace Digikam

#endif // KD_NODE_H
