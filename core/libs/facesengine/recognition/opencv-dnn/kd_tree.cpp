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

#include "kd_tree.h"

namespace Digikam
{

class KDTree::Private
{
public:
    Private(int dim)
        : nbDimension(dim),
          root(nullptr)
    {
    }

    ~Private()
    {
        nodes.clear();
    }

public:

    int nbDimension;
    KDNode* root;
    QVector<KDNode*> nodes;
};

KDTree::KDTree(int dim)
    : d(new Private(dim))
{
}

KDTree::~KDTree()
{
    delete d;
}

KDNode* KDTree::add(const cv::Mat& position, const int identity)
{
    if (d->root == nullptr)
    {
        d->root = new KDNode(position, identity, 0, d->nbDimension);
        d->nodes.append(d->root);

        return d->root;
    }
    else
    {
        KDNode* pNode = nullptr;
        if ((pNode = d->root->insert(position, identity)) != nullptr)
        {
            d->nodes.append(pNode);

            return pNode;
        }
    }

    return nullptr;
}

QMap<double, QVector<KDNode*> > KDTree::getClosestNeighbors(const cv::Mat& position, double sqRange, int maxNbNeighbors) const
{
    QMap<double, QVector<KDNode*> > closestNeighbors;

    if (d->root)
    {
        sqRange = d->root->getClosestNeighbors(closestNeighbors, position, sqRange, maxNbNeighbors);
    }

    return closestNeighbors;
}

} // namespace RecognitionTest
