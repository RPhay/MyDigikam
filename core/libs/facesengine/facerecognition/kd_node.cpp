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

#include "kd_node.h"

// std include
#include <cfloat>

// Qt include
#include <QtMath>

using namespace Digikam;

namespace RecognitionTest
{

class Q_DECL_HIDDEN KDNode::Private
{
public:

    Private(std::vector<double> nodePos, const Identity& identity, int splitAxis, int dimension)
        : identity(identity),
          splitAxis(splitAxis),
          nbDimension(dimension),
          position(nodePos),
          maxRange(nodePos),
          minRange(nodePos),
          parent(nullptr),
          left(nullptr),
          right(nullptr)
    {
    }

    ~Private()
    {
        delete left;
        delete right;
    }

public:

    Identity identity;

    int splitAxis;
    int nbDimension;
    std::vector<double> position;
    std::vector<double> maxRange;
    std::vector<double> minRange;
    KDNode* parent;
    KDNode* left;
    KDNode* right;
};


double KDNode::sqrDistance(std::vector<double> pos1, std::vector<double> pos2)
{
    Q_ASSERT(pos1.size() == pos2.size());

    double sqrDistance = 0;

    for (size_t i = 0; i < pos1.size(); ++i)
    {
        sqrDistance += pow((pos1[i] - pos2[i]), 2);
    }

    return sqrDistance;
}

KDNode::KDNode(const std::vector<double>& nodePos, const Identity& identity, int splitAxis, int dimension)
    : d(new Private(nodePos, identity, splitAxis, dimension))
{
    Q_ASSERT(splitAxis < dimension);
    Q_ASSERT(int(nodePos.size()) == dimension);
}

KDNode::~KDNode()
{
    delete d;
}

KDNode* KDNode::insert(const std::vector<double>& nodePos, const Identity& identity)
{
    if (int(nodePos.size()) != d->nbDimension)
    {
        return nullptr;
    }

    KDNode* parent = findParent(nodePos);

    if (nodePos == parent->getPosition())
    {
        return nullptr;
    }

    KDNode* newNode = new KDNode(nodePos, identity,
                                 ((parent->d->splitAxis + 1) % d->nbDimension),
                                 d->nbDimension);
    newNode->d->parent = parent;

    if (nodePos[parent->d->splitAxis] >= parent->getPosition()[parent->d->splitAxis])
    {
        parent->d->right = newNode;
    }
    else
    {
        parent->d->left = newNode;
    }

    return newNode;
}

std::vector<double> KDNode::getPosition() const
{
    return d->position;
}

Identity& KDNode::getIdentity()
{
    return d->identity;
}

double KDNode::getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                                   std::vector<double> position,
                                   double sqRange,
                                   int maxNbNeighbors)
{
    // add current node to the list
    double distanceToCurrentNode = sqrt(sqrDistance(position, d->position));

    neighborList[distanceToCurrentNode].append(this);

    // limit the size of the Map to maxNbNeighbors
    int size = 0;

    for (QMap<double, QVector<KDNode*> >::const_iterator iter  = neighborList.cbegin();
                                                         iter != neighborList.cend();
                                                       ++iter)
    {
        size += iter.value().size();
    }

    if (size > maxNbNeighbors)
    {
        // Eliminate the farthest neighbor
        QMap<double, QVector<KDNode*> >::iterator farthestNodes = (neighborList.end() - 1);

        if (farthestNodes.value().size() == 1)
        {
            neighborList.erase(farthestNodes);
        }
        else
        {
            farthestNodes.value().pop_back();
        }

        // update the searching range
        sqRange = pow(neighborList.lastKey(), 2);
    }

    // sub-trees Traversal
    double sqrDistanceleftTree  = 0;


    if (d->left == nullptr)
    {
        sqrDistanceleftTree = DBL_MAX;
    }
    else
    {
        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistanceleftTree += (pow(qMax(0.0, (d->left->d->minRange[i] - position[i])), 2) +
                                    pow(qMax(0.0, (position[i] - d->left->d->maxRange[i])), 2));
        }
    }


    double sqrDistancerightTree = 0;

    if (d->right == nullptr)
    {
        sqrDistancerightTree = DBL_MAX;
    }
    else
    {
        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistancerightTree += (pow(qMax(0.0, (d->right->d->minRange[i] - position[i])), 2) +
                                     pow(qMax(0.0, (position[i] - d->right->d->maxRange[i])), 2));
        }
    }

    // traverse the closest area
    if (sqrDistanceleftTree < sqrDistancerightTree)
    {
        if (sqrDistanceleftTree < sqRange)
        {
            // traverse left Tree
            sqRange = d->left->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistancerightTree < sqRange)
            {
                // traverse right Tree
                sqRange = d->right->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }
    else
    {
        if (sqrDistancerightTree < sqRange)
        {
            // traverse right Tree
            sqRange = d->right->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistanceleftTree < sqRange)
            {
                // traverse left Tree
                sqRange = d->left->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }

    return sqRange;
}

void KDNode::updateRange(std::vector<double> pos)
{
    if (int(pos.size()) != d->nbDimension)
    {
        return;
    }

    for (int i = 0; i < d->nbDimension; i++)
    {
        d->maxRange[i] = std::max(d->maxRange[i], pos[i]);
        d->minRange[i] = std::min(d->minRange[i], pos[i]);
    }
}

KDNode* KDNode::findParent(std::vector<double> nodePos)
{
    KDNode* parent = nullptr;
    KDNode* currentNode = this;

    while (currentNode != nullptr)
    {
        currentNode->updateRange(nodePos);

        int split = currentNode->d->splitAxis;
        parent    = currentNode;

        if (nodePos[split] >= currentNode->d->position[split])
        {
            currentNode = currentNode->d->right;
        }
        else
        {
            currentNode = currentNode->d->left;
        }
    }

    return parent;
}

}
