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

double sqrDistance(const float* pos1, const float* pos2, int dimension)
{
    double sqrDistance = 0;

    for (int i = 0; i < dimension; ++i)
    {
        sqrDistance += pow((pos1[i] - pos2[i]), 2);
    }

    return sqrDistance;
}

class Q_DECL_HIDDEN KDNode::Private
{
public:

    Private(cv::Mat nodePos, const Identity& identity, int splitAxis, int dimension)
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
    cv::Mat position;
    cv::Mat maxRange;
    cv::Mat minRange;
    KDNode* parent;
    KDNode* left;
    KDNode* right;
};

KDNode::KDNode(const cv::Mat& nodePos, const Identity& identity, int splitAxis, int dimension)
    : d(new Private(nodePos, identity, splitAxis, dimension))
{
    Q_ASSERT(splitAxis < dimension);
    Q_ASSERT(nodePos.cols == 1 && nodePos.rows == dimension && nodePos.type() == CV_32F);
}

KDNode::~KDNode()
{
    delete d;
}

KDNode* KDNode::insert(const cv::Mat& nodePos, const Identity& identity)
{
    if (!(nodePos.cols == 1 && nodePos.rows == d->nbDimension && nodePos.type() == CV_32F))
    {
        return nullptr;
    }

    KDNode* parent = findParent(nodePos);
/*
    if (nodePos == parent->getPosition())
    {
        return nullptr;
    }
*/
    KDNode* newNode = new KDNode(nodePos, identity,
                                 ((parent->d->splitAxis + 1) % d->nbDimension),
                                 d->nbDimension);
    newNode->d->parent = parent;

    if (nodePos.at<float>(0, parent->d->splitAxis) >= parent->getPosition().at<float>(0, parent->d->splitAxis))
    {
        parent->d->right = newNode;
    }
    else
    {
        parent->d->left = newNode;
    }

    return newNode;
}

cv::Mat KDNode::getPosition() const
{
    return d->position;
}

Identity& KDNode::getIdentity()
{
    return d->identity;
}

double KDNode::getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                                   const cv::Mat& position,
                                   double sqRange,
                                   int maxNbNeighbors)
{
    // add current node to the list
    double sqrdistanceToCurrentNode = sqrDistance(position.ptr<float>(), d->position.ptr<float>(), d->nbDimension);

    neighborList[sqrdistanceToCurrentNode].append(this);

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
        sqRange = neighborList.lastKey();
    }

    // sub-trees Traversal
    double sqrDistanceleftTree  = 0;

    if (d->left == nullptr)
    {
        sqrDistanceleftTree = DBL_MAX;
    }
    else
    {
        const float* minRange = d->left->d->minRange.ptr<float>();
        const float* maxRange = d->left->d->maxRange.ptr<float>();
        const float* pos = position.ptr<float>();

        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistanceleftTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                    pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
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
            const float* minRange = d->right->d->minRange.ptr<float>();
            const float* maxRange = d->right->d->maxRange.ptr<float>();
            const float* pos = position.ptr<float>();

            sqrDistancerightTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                     pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
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

void KDNode::updateRange(const cv::Mat& pos)
{
    if (!(pos.cols == 1 && pos.rows == d->nbDimension && pos.type() == CV_32F))
    {
        return;
    }

    float* minRange = d->right->d->minRange.ptr<float>();
    float* maxRange = d->right->d->maxRange.ptr<float>();
    const float* position = pos.ptr<float>();

    for (int i = 0; i < d->nbDimension; ++i)
    {
        maxRange[i] = std::max(maxRange[i], position[i]);
        minRange[i] = std::min(minRange[i], position[i]);
    }
}

KDNode* KDNode::findParent(const cv::Mat& nodePos)
{
    KDNode* parent = nullptr;
    KDNode* currentNode = this;

    while (currentNode != nullptr)
    {
        currentNode->updateRange(nodePos);

        int split = currentNode->d->splitAxis;
        parent    = currentNode;

        if (nodePos.at<float>(0, split) >= currentNode->d->position.at<float>(0, split))
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
