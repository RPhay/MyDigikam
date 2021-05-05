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

// Qt includes

#include <QMutex>

namespace Digikam
{

class Q_DECL_HIDDEN KDTree::Private
{

public:

    Private(int dim)
        : nbDimension   (dim),
          root          (nullptr)
    {
    }

    ~Private()
    {
        delete root;
    }

public:

    int     nbDimension;
    KDNode* root;
    QMutex  mutex;
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
    KDNode* newNode = nullptr;

    d->mutex.lock();
    {
        if (d->root == nullptr)
        {
            d->root = new KDNode(position, identity, 0, d->nbDimension);

            newNode = d->root;
        }
        else
        {
            newNode = d->root->insert(position, identity);
        }
    }
    d->mutex.unlock();

    return newNode;
}

QMap<double, QVector<int> > KDTree::getClosestNeighbors(const cv::Mat& position,
                                                        float sqRange,
                                                        float cosThreshold,
                                                        int maxNbNeighbors) const
{
    QMap<double, QVector<int> > closestNeighbors;

    if (d->root)
    {
        d->root->getClosestNeighbors(closestNeighbors, position, sqRange, cosThreshold, maxNbNeighbors);
    }

    return closestNeighbors;
}

} // namespace Digikam
