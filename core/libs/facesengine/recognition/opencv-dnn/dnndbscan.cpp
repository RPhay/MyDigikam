/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-11
 * Description : Class performs DBSCAN for clustering with DNN recognition
 *               More on DBSCAN (Density-Based Spatial Clustering of Applications with Noise):
 *               https://medium.com/@elutins/dbscan-what-is-it-when-to-use-it-how-to-use-it-8bd506293818
 *               https://github.com/bowbowbow/DBSCAN/blob/master/clustering.cpp
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dnndbscan.h"

// C includes

#include <stddef.h>

// C++ includes

#include <limits>
#include <cmath>
#include <cassert>
#include <numeric>

namespace Digikam
{

static const int NOT_CLASSIFIED = -1;
static const int NOISE          = -2;

// -----------------------------------------------------------------------------------------

double s_DNNDbscanCosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2)
{
    assert(v1.size() == v2.size());

    double scalarProduct = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0);
    double normV1        = sqrt(std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0));
    double normV2        = sqrt(std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0));

    return (scalarProduct / (normV1 * normV2));
}

// -----------------------------------------------------------------------------------------

DNNDbscanPointCustomized::DNNDbscanPointCustomized(const std::vector<float>& _data)
  : ptsCount(0),
    cluster(NOT_CLASSIFIED),
    data(_data)
{
}

double DNNDbscanPointCustomized::computeSimilarity(const DNNDbscanPointCustomized& p)
{
    return s_DNNDbscanCosineSimilarity(p.data, data);
}

// -----------------------------------------------------------------------------------------

DNNDbscan::DNNDbscan(double eps,
                     int minPts,
                     const std::vector<DNNDbscanPointCustomized>& pts)
  : eps(eps),
    minPtsPerCluster(minPts),
    points(pts),
    nbOfClusters(-1)
{
}

/*
void DNNDbscan::run()
{
    checkNearPoints();

    for (size_t i = 0 ; i < points.size() ; ++i)
    {

        if (points[i].cluster != NOT_CLASSIFIED)
        {
            continue;
        }

        if (isCoreObject(i))
        {
            dfs(i, ++nbOfClusters);
        }
        else
        {
            points[i].cluster = NOISE;
        }
    }

    ++nbOfClusters; // since nbOfClusters counted from 0

    for (size_t i = 0 ; i < points.size() ; ++i)
    {
        int cluster = points[i].cluster;

        if (cluster != NOISE)
        {
            clusteredIdx.push_back(cluster);
        }
        else
        {
            clusteredIdx.push_back(nbOfClusters);
        }
    }

    ++nbOfClusters; // Noise is added as last group
}
*/

void DNNDbscan::run()
{
    for (size_t i = 0 ; i < points.size() ; ++i)
    {
        if (points[i].cluster != NOT_CLASSIFIED)
        {
            continue;
        }

        std::list<int> neighbors;
        findNeighbors(points[i], i, neighbors);

        if ((int)neighbors.size() < minPtsPerCluster)
        {
            points[i].cluster = NOISE;
            continue;
        }

        ++nbOfClusters;

        points[i].cluster = nbOfClusters;

        while (!neighbors.empty())
        {
            int pointIdx = neighbors.front();
            neighbors.pop_front();

            if (points[pointIdx].cluster == NOISE)
            {
                points[pointIdx].cluster = nbOfClusters;
                continue;
            }
            else if (points[pointIdx].cluster != NOT_CLASSIFIED)
            {
                continue;
            }

            points[pointIdx].cluster = nbOfClusters;

            std::list<int> newNeighbors;
            findNeighbors(points[pointIdx], pointIdx, newNeighbors);

            if ((int)newNeighbors.size() >= minPtsPerCluster)
            {
                std::copy(newNeighbors.begin(), newNeighbors.end(), std::back_inserter(neighbors));
            }
        }
    }

    ++nbOfClusters; // since nbOfClusters counted from 0

    for (size_t i = 0 ; i < points.size() ; ++i)
    {
        int cluster = points[i].cluster;

        if (cluster != NOISE)
        {
            clusteredIdx.push_back(cluster);
        }
        else
        {
            clusteredIdx.push_back(nbOfClusters);
        }
    }

    ++nbOfClusters; // Noise is added as last group
}

void DNNDbscan::dfs(int current, int cluster)
{
    points[current].cluster = cluster;

    if (!isCoreObject(current))
    {
        return;
    }

    for (auto& next: adjPoints[current])
    {
        if (points[next].cluster != NOT_CLASSIFIED)
        {
            continue;
        }

        dfs(next, cluster);
    }
}

void DNNDbscan::checkNearPoints()
{
    for (size_t i = 0 ; i < points.size() ; ++i)
    {
        std::vector<int> vecAdjToPoint;

        for (size_t j = 0 ; j < points.size() ; ++j)
        {
            if (i == j)
            {
                continue;
            }

            if (points[i].computeSimilarity(points[j]) >= eps) // since it is cosine similarity
            {
                points[i].ptsCount++;
                vecAdjToPoint.push_back(j);
            }
        }

        adjPoints.push_back(vecAdjToPoint);
    }
}

// Core object has at least minPtsPerCluster as neighbors

bool DNNDbscan::isCoreObject(int idx)
{
    return (points[idx].ptsCount >= minPtsPerCluster);
}

void DNNDbscan::findNeighbors(const DNNDbscanPointCustomized& p, int pointIdx, std::list<int>& neighbors)
{
    for (unsigned int i = 0 ; i < points.size() ; ++i)
    {
        if ((points[i].computeSimilarity(p) >= eps) && ((int)i != pointIdx))
        {
            neighbors.push_back(i);
        }
    }
}

int DNNDbscan::getCluster(std::vector<int>& clusteredIndices)
{
    if (clusteredIndices.empty() ||
        (clusteredIndices.size() < clusteredIdx.size()))
    {
        clusteredIndices.shrink_to_fit();
        std::copy(clusteredIdx.begin(), clusteredIdx.end(), std::back_inserter(clusteredIndices));
    }
    else
    {
        std::copy(clusteredIdx.begin(), clusteredIdx.end(), clusteredIndices.begin());
    }

    return nbOfClusters;
}

} // namespace Digikam
