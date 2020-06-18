/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Implementation of KD-Tree for vector space partitioning
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

#ifndef KD_TREE_H
#define KD_TREE_H

#include "kd_node.h"

namespace RecognitionTest
{

class KDTree
{
public:

    explicit KDTree(int dim);
    ~KDTree();

    /**
     *
     * @param position
     * @param sqRange
     * @param maxNbNeighbors
     * @return Map of N-nearest neighbors, sorted by distance
     */
    QMap<double, QVector<KDNode*> > getClosestNeighbors(const std::vector<float>& position, double sqRange, int maxNbNeighbors);

    void update();

    bool add(const std::vector<float>& position, const Digikam::Identity& identity);

private:

    class Private;
    Private* d;
};

}

#endif // KD_TREE_H
