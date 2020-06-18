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

// Local include
#include "identity.h"

using namespace Digikam;

namespace RecognitionTest
{

class KDNode
{
public:

    explicit KDNode(const std::vector<float>& nodePos, const Identity& identity, int splitAxis, int dimension);
    ~KDNode();

public:

    KDNode* insert(const std::vector<float>& nodePos, const Identity& identity);

    std::vector<float> getPosition() const;

    double getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                               std::vector<float> position,
                               double sqRange,
                               int maxNbNeighbors);
    Identity& getIdentity();

private:

    void updateRange(std::vector<float>);

    KDNode* findParent(std::vector<float> nodePos);

    //static double sqrDistance(std::vector<float> pos1, std::vector<float> pos2);

private:

    class Private;
    Private* d;
};

}

#endif // KD_NODE_H
