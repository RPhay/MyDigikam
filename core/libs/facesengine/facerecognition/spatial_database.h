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

#include <vector>

namespace RecognitionTest
{

class SpatialDatabase
{
public:

    explicit SpatialDatabase();
    ~SpatialDatabase();

public:

    bool insert(const std::vector<float>& nodePos, const int label);

private:

    bool updateRange(int nodeId, std::vector<float>& minRange, std::vector<float>& maxRange, const std::vector<float>& position);

    int findParent(const std::vector<float>& nodePos, bool& leftChild, int& parentSplitAxis);

private:

    class Private;
    Private* d;
};
}

#endif // SPATIAL_DATABASE_H
