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

#include "kd_treebase.h"

// Qt includes

#include <QMutex>

namespace Digikam
{

class Q_DECL_HIDDEN KDTreeBase::Private
{

public:

    Private(int dim, int threshold)
        : nbDimension(dim),
          mapThreshold(threshold)
    {
    }

    ~Private()
    {
        delete root;
    }

public:

    int                  nbDimension    = 0;
    KDNodeBase*          root           = nullptr;
    QMutex               mutex;

    QVector<KDNodeBase*> nodeMap;
    int                  mapThreshold   = KDTREE_MAP_THRESHOLD;    ///< Above this size start using the KDTree instead of the vector.
    bool                 useMap         = true;

};

KDTreeBase::KDTreeBase(
    int dim,
    int threshold = KDTREE_MAP_THRESHOLD        ///< If the vector grows to 500 items, start using the KDTree.
)
    : d(new Private(dim, threshold))
{
    /**
     * Using this to compare brute force vs kdtree performance due to sparse data
     * in k-dimensions (128 dimensions for face features).
     */

    QString bruteForce = QString::fromLocal8Bit(qgetenv("DIGIKAM_KDREE_USEBRUTEFORCE"));

    if (bruteForce.length() > 0)
    {
        if (
            QString::fromUtf8("1")    == bruteForce ||
            QString::fromUtf8("true") == bruteForce.toLower()
        )
        {
            d->mapThreshold = std::numeric_limits<int>::max();      // Set the vectorThreshold so high we always use the vector.
        }
    }
}

KDTreeBase::~KDTreeBase()
{
    delete d;
}

KDNodeBase* KDTreeBase::add(const cv::Mat& position, const int identity)
{
    KDNodeBase* newNode = nullptr;

    d->mutex.lock();
    {
        if (d->root == nullptr)
        {
            d->root = createNode(position, identity, 0, d->nbDimension);

            newNode = d->root;
        }

        else
        {
            newNode = d->root->insert(position, identity);
        }

        // To avoid issues with sparse density in the tree we initially use a vector of nodes
        // and compare all targets to the samples in the vector.  When sufficient data density
        // has been achieved, we delete the vector (but not the nodes), and begin using the tree
        // for classification.

        if (d->useMap)
        {
            if (d->nodeMap.size() < d->mapThreshold)
            {
                d->nodeMap.append(newNode);
            }

            else
            {
                d->useMap = false;
                d->nodeMap.clear();     // Don't delete the nodes. The nodes are also used by the tree.
            }
        }
    }
    d->mutex.unlock();

    return newNode;
}

QMap<double, QVector<int> > KDTreeBase::getClosestNeighbors(const cv::Mat& position,
                                                            float sqRange,
                                                            int maxNbNeighbors) const
{
    QMap<double, QVector<int> > closestNeighbors;

    // To avoid issues with sparse density in the tree we initially use a vector of nodes
    // and compare all targets to the sample nodes in the vector. When sufficient data density
    // has been achieved, we delete the vector (but not the nodes), and begin using the tree
    // for classification.

    if (d->useMap)
    {
        QVector<KDNodeBase*>::iterator node = d->nodeMap.begin();

        while (node != d->nodeMap.end())
        {
            KDNodeBase::NodeCompareResult result = (*node)->nodeCompare(position,
                                                                        (*node)->getPosition(),
                                                                        sqRange,
                                                                        getCosThreshold(sqRange),
                                                                        (*node)->getDimension());

            if (result.result)
            {
                closestNeighbors[result.distance1].append((*node)->getIdentity());
            }

            ++node;
        }
    }

    else
    {
        if (d->root)
        {
            d->root->getClosestNeighbors(closestNeighbors, position, sqRange, getCosThreshold(sqRange), maxNbNeighbors);
        }
    }

    return closestNeighbors;
}

} // namespace Digikam
