/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-07-06
 * Description : Implementation of spatial database for KD-Tree storage
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

#ifndef SPATIAL_DATABASE_H
#define SPATIAL_DATABASE_H

// Std includes
#include <vector>
// Qt include
#include <QMap>

namespace RecognitionTest
{

class DataNode
{
public:
    explicit DataNode()
        : nodeID(0),
          label(0),
          splitAxis(0)
    {
    }

    bool isNull() const
    {
        return (nodeID == 0);
    }

public:
    int nodeID;
    int label;
    int splitAxis;
    int left;
    int right;
    std::vector<float> position;
    std::vector<float> minRange;
    std::vector<float> maxRange;
};

class SpatialDatabase
{
public:

    explicit SpatialDatabase();
    ~SpatialDatabase();

public:
    /**
     * @brief insert : insert a new node to database
     * @param nodePos
     * @param label
     * @return true if successed
     */
    bool insert(const std::vector<float>& nodePos, const int label);

    /**
     * @brief getClosestNeighbors : return a list of closest neighbor, limited by maxNbNeighbors and sqRange
     * @param subTree
     * @param neighborList
     * @param position
     * @param sqRange
     * @param maxNbNeighbors

     * @return
     */
    QMap<double, QVector<int> > getClosestNeighbors(std::vector<float> position,
                                                    double sqRange,
                                                    int maxNbNeighbors);

private:

    bool updateRange(int nodeId, std::vector<float>& minRange, std::vector<float>& maxRange, const std::vector<float>& position);

    int findParent(const std::vector<float>& nodePos, bool& leftChild, int& parentSplitAxis);

    /**
     * @brief getClosestNeighbors : return a list of closest neighbor from a sub tree, limited by maxNbNeighbors and sqRange
     * @param subTree
     * @param neighborList
     * @param position
     * @param sqRange
     * @param maxNbNeighbors

     * @return
     */
    double getClosestNeighbors(const DataNode& subTree,
                               QMap<double, QVector<int> >& neighborList,
                               std::vector<float> position,
                               double sqRange,
                               int maxNbNeighbors);
private:

    class Private;
    Private* d;
};

}

#endif // SPATIAL_DATABASE_H
