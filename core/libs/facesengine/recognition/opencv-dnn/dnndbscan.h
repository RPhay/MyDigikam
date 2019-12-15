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

#ifndef DIGIKAM_DNN_DBSCAN_H
#define DIGIKAM_DNN_DBSCAN_H

// C++ includes

#include <vector>
#include <list>

namespace Digikam
{

/**
 * This compute cosine distance between 2 vectors with formula:
 * cos(a) = (v1*v2) / (||v1||*||v2||)
 */
double s_DNNDbscanCosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2);

class DNNDbscanPointCustomized
{

public:

    explicit DNNDbscanPointCustomized(const std::vector<float>& _data);
    ~DNNDbscanPointCustomized() = default;

    double computeSimilarity(const DNNDbscanPointCustomized& ot);

public:

    int                ptsCount;
    int                cluster;

private:

    std::vector<float> data;
};

// ------------------------------------------------------------------------------------------

class DNNDbscan
{

public:

    explicit DNNDbscan(double eps,
                       int minPts,
                       const std::vector<DNNDbscanPointCustomized>& pts);
    ~DNNDbscan() = default;

    void run();
    int getCluster(std::vector<int>& clusteredIndices);

private:

    void dfs(int current, int cluster);
    void checkNearPoints();
    bool isCoreObject(int idx);

    void findNeighbors(const DNNDbscanPointCustomized& p,
                       int pointIdx,
                       std::list<int>& neighbors);

private:

    double                                eps;
    int                                   minPtsPerCluster;

    std::vector<DNNDbscanPointCustomized> points;
    std::vector<std::vector<int>>         adjPoints;

    int                                   nbOfClusters;
    std::vector<int>                      clusteredIdx;
};

} // namespace Digikam

#endif // DIGIKAM_DNN_DBSCAN_H
