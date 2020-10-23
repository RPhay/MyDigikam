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

// C++ include

#include <cfloat>
#include <cstdio>

// Qt include

#include <QtMath>
#include <QDebug>

// Local includes

#include "dnnfaceextractor.h"

namespace Digikam
{

float KDNode::sqrDistance(const float* const pos1, const float* const pos2, int dimension)
{
    double sqrDistance = 0.0;

    for (int i = 0 ; i < dimension ; ++i)
    {
        sqrDistance += pow((pos1[i] - pos2[i]), 2);
    }

    return float(sqrDistance);
}

float KDNode::cosDistance(const float* const pos1, const float* const pos2, int dimension)
{
    double scalarProduct = 0.0;
    double normV1        = 0.0;
    double normV2        = 0.0;

    for (int i = 0 ; i < dimension ; ++i)
    {
        scalarProduct += pos1[i] * pos2[i];
        normV1        += pow(pos1[i], 2);
        normV2        += pow(pos2[i], 2);
    }

    return float(scalarProduct / (normV1 * normV2));
}

// ----------------------------------------------------------------------------------------

class Q_DECL_HIDDEN KDNode::Private
{
public:

    Private(const cv::Mat& nodePos, const int identity, int splitAxis, int dimension)
        : nodeID        (-1),
          identity      (identity),
          splitAxis     (splitAxis),
          nbDimension   (dimension),
          position      (nodePos.clone()),
          maxRange      (nodePos.clone()),
          minRange      (nodePos.clone()),
          parent        (nullptr),
          left          (nullptr),
          right         (nullptr)
    {
    }

    ~Private()
    {
        delete left;
        delete right;
    }

public:

    int     nodeID;
    int     identity;
    int     splitAxis;
    int     nbDimension;

    cv::Mat position;
    cv::Mat maxRange;
    cv::Mat minRange;
    KDNode* parent;
    KDNode* left;
    KDNode* right;
};

KDNode::KDNode(const cv::Mat& nodePos,
               const int      identity,
               int            splitAxis,
               int            dimension)
    : d(new Private(nodePos, identity, splitAxis, dimension))
{
    Q_ASSERT(splitAxis < dimension);
    Q_ASSERT((nodePos.rows   == 1)         &&
             (nodePos.cols   == dimension) &&
             (nodePos.type() == CV_32F));
}

KDNode::~KDNode()
{
    delete d;
}

KDNode* KDNode::insert(const cv::Mat& nodePos, const int identity)
{
    if (!((nodePos.rows   == 1)              &&
          (nodePos.cols   == d->nbDimension) &&
          (nodePos.type() == CV_32F)))
    {
        return nullptr;
    }

    KDNode* parent  = findParent(nodePos);

    KDNode* newNode = new KDNode(nodePos, identity,
                                 ((parent->d->splitAxis + 1) % d->nbDimension),
                                 d->nbDimension);
    newNode->d->parent = parent;
/*
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "parent embedding" << parent->getPosition() << std::endl;
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "node embedding" << nodePos << std::endl;
*/
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

int KDNode::getIdentity()
{
    return d->identity;
}

void KDNode::setNodeId(int id)
{
    d->nodeID = id;
}

double KDNode::getClosestNeighbors(QMap<double, QVector<int> >& neighborList,
                                   const cv::Mat&               position,
                                   float                        sqRange,
                                   float                        cosThreshold,
                                   int                          maxNbNeighbors) const
{
    // add current node to the list

    const double sqrDistanceToCurrentNode = sqrDistance(position.ptr<float>(), d->position.ptr<float>(), d->nbDimension);

    // NOTE: both Euclidian distance and cosine distance can help to avoid error in similarity prediction

    if ((sqrDistanceToCurrentNode < sqRange) &&
        (cosDistance(position.ptr<float>(), d->position.ptr<float>(), d->nbDimension) > cosThreshold))
    {
        neighborList[sqrDistanceToCurrentNode].append(d->identity);

        // limit the size of the Map to maxNbNeighbors

        int size = 0;

        for (QMap<double, QVector<int> >::const_iterator iter  = neighborList.cbegin();
                                                         iter != neighborList.cend();
                                                         ++iter)
        {
            size += iter.value().size();
        }

        if (size > maxNbNeighbors)
        {
            // Eliminate the farthest neighbor

            QMap<double, QVector<int> >::iterator farthestNodes = (neighborList.end() - 1);

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
    }

    // sub-trees Traversal
    // NOTE: DBL_MAX helps avoiding accessing nullptr

    double sqrDistanceleftTree  = 0.0;

    if (d->left == nullptr)
    {
        sqrDistanceleftTree = DBL_MAX;
    }
    else
    {
        const float* const minRange = d->left->d->minRange.ptr<float>();
        const float* const maxRange = d->left->d->maxRange.ptr<float>();
        const float* const pos      = position.ptr<float>();

        for (int i = 0 ; i < d->nbDimension ; ++i)
        {
            sqrDistanceleftTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                    pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
        }
    }

    double sqrDistancerightTree = 0.0;

    if (d->right == nullptr)
    {
        sqrDistancerightTree = DBL_MAX;
    }
    else
    {
        const float* const minRange = d->right->d->minRange.ptr<float>();
        const float* const maxRange = d->right->d->maxRange.ptr<float>();
        const float* const pos      = position.ptr<float>();

        for (int i = 0 ; i < d->nbDimension ; ++i)
        {
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

            if (d->left)
            {
                sqRange = d->left->getClosestNeighbors(neighborList, position, sqRange, cosThreshold, maxNbNeighbors);

                if (sqrDistancerightTree < sqRange)
                {
                    // traverse right Tree

                    if (d->right)
                    {
                        sqRange = d->right->getClosestNeighbors(neighborList, position, sqRange, cosThreshold, maxNbNeighbors);
                    }
                }
            }
        }
    }
    else
    {
        if (sqrDistancerightTree < sqRange)
        {
            // traverse right Tree

            if (d->right)
            {
                sqRange = d->right->getClosestNeighbors(neighborList, position, sqRange, cosThreshold, maxNbNeighbors);

                if (sqrDistanceleftTree < sqRange)
                {
                    // traverse left Tree

                    if (d->left)
                    {
                        sqRange = d->left->getClosestNeighbors(neighborList, position, sqRange, cosThreshold, maxNbNeighbors);
                    }
                }
            }
        }
    }

    return sqRange;
}

void KDNode::updateRange(const cv::Mat& pos)
{
    if (!((pos.rows   == 1)              &&
          (pos.cols   == d->nbDimension) &&
          (pos.type() == CV_32F)))
    {
        return;
    }

    float* minRange       = d->minRange.ptr<float>();
    float* maxRange       = d->maxRange.ptr<float>();
    const float* position = pos.ptr<float>();

    for (int i = 0 ; i < d->nbDimension ; ++i)
    {
        maxRange[i] = std::max(maxRange[i], position[i]);
        minRange[i] = std::min(minRange[i], position[i]);
    }
}

KDNode* KDNode::findParent(const cv::Mat& nodePos)
{
    KDNode* parent      = nullptr;
    KDNode* currentNode = this;

    while (currentNode != nullptr)
    {
        currentNode->updateRange(nodePos);

        int split       = currentNode->d->splitAxis;
        parent          = currentNode;

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

} // namespace Digikam
