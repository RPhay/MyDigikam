/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface for spatial storage of face embedding.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "facedb_p.h"

namespace Digikam
{

class FaceDb::DataNode
{
public:

    explicit DataNode()
        : nodeID    (0),
          label     (0),
          splitAxis (0),
          left      (-1),
          right     (-1)
    {
    }

    bool isNull() const
    {
        return (nodeID == 0);
    }

public:

    int     nodeID;
    int     label;
    int     splitAxis;
    int     left;
    int     right;
    cv::Mat position;
    cv::Mat minRange;
    cv::Mat maxRange;
};

bool FaceDb::insertToTreeDb(const int nodeID, const cv::Mat& faceEmbedding) const
{
    bool isLeftChild    = false;
    int parentSplitAxis = 0;
    int parentID        = findParentTreeDb(faceEmbedding, isLeftChild, parentSplitAxis);

    if (parentID < 0)
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "fail to find parent node";
        return false;
    }

    QVariantList bindingValues;

    bindingValues << (parentSplitAxis + 1) % 128;
    bindingValues << nodeID;
    bindingValues << QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * 128));
    bindingValues << QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * 128));
    bindingValues << parentID;

    // insert node to database

    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("INSERT INTO KDTree "
                                                            "(split_axis, position, max_range, min_range, parent, `left`, `right`) "
                                                            "VALUES (?, ?, ?, ?, ?, NULL, NULL)"),
                                              bindingValues);

    int newNode            = query.lastInsertId().toInt();

    if (newNode <= 0)
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "error insert into treedb" << query.lastError();
    }

    if (parentID > 0)
    {
        bindingValues.clear();
        bindingValues << newNode;
        bindingValues << parentID;

        // not root -> update parent

        if (isLeftChild)
        {
            query = d->db->execQuery(QLatin1String("UPDATE KDTree SET left = ? WHERE id = ?;"), bindingValues);
        }
        else
        {
            query = d->db->execQuery(QLatin1String("UPDATE KDTree SET right = ? WHERE id = ?;"), bindingValues);
        }
    }

    return true;
}

QMap<double, QVector<int> > FaceDb::getClosestNeighborsTreeDb(const cv::Mat& position,
                                                              float sqRange,
                                                              float cosThreshold,
                                                              int maxNbNeighbors) const
{
    QMap<double, QVector<int> > closestNeighbors;

    DataNode root;

    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT position, max_range, min_range, `left`, `right` "
                                                            "FROM KDTree WHERE id = 1"));
    if (query.next())
    {
        // encapsulate data node

        root.nodeID     = 1;
        int embeddingID = query.value(0).toInt();
        root.maxRange   = cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone();
        root.minRange   = cv::Mat(1, 128, CV_32F, query.value(2).toByteArray().data()).clone();
        root.left       = query.value(3).toInt();
        root.right      = query.value(4).toInt();

        QVariantList bindingValues;
        bindingValues << embeddingID;

        query = d->db->execQuery(QLatin1String("SELECT identity, embedding FROM FaceMatrices WHERE id = ?"),
                                 bindingValues);

        if (query.next())
        {
            root.label    = query.value(0).toInt();
            root.position = cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone();
        }

        getClosestNeighborsTreeDb(root, closestNeighbors, position, sqRange, cosThreshold, maxNbNeighbors);
    }

    return closestNeighbors;
}

void FaceDb::clearTreeDb() const
{
    d->db->execSql(QLatin1String("DELETE FROM KDTree;"));
}

void FaceDb::updateRangeTreeDb(int nodeId, cv::Mat& minRange, cv::Mat& maxRange, const cv::Mat& position) const
{
    float* const min = minRange.ptr<float>();
    float* const max = maxRange.ptr<float>();
    const float* pos = position.ptr<float>();

    for (int i = 0 ; i < position.cols ; ++i)
    {
        max[i] = std::max(max[i], pos[i]);
        min[i] = std::min(min[i], pos[i]);
    }

    QVariantList bindingValues;

    bindingValues << QByteArray::fromRawData((char*)max, (sizeof(float) * 128));
    bindingValues << QByteArray::fromRawData((char*)min, (sizeof(float) * 128));
    bindingValues << nodeId;

    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("UPDATE KDTree SET max_range = ?, min_range = ? WHERE id = ?;"),
                                              bindingValues);
}

int FaceDb::findParentTreeDb(const cv::Mat& nodePos, bool& leftChild, int& parentSplitAxis) const
{
    int parent           = 1;
    QVariant currentNode = parent;

    while (currentNode.isValid() && !currentNode.isNull())
    {
        parent = currentNode.toInt();

        QVariantList bindingValues;
        bindingValues << parent;

        DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT split_axis, position, max_range, min_range, `left`, `right` "
                                                                "FROM KDTree WHERE id = ?"),
                                                  bindingValues);

        if (! query.next())
        {
            if (parent == 1)
            {
/*
                qCDebug(DIGIKAM_FACEDB_LOG) << "add root";
*/
                return 0;
            }

            qCWarning(DIGIKAM_FACEDB_LOG) << "Error query parent =" << parent << query.lastError();
            return -1;
        }

/*
        qCDebug(DIGIKAM_FACEDB_LOG) << "split axis" << query.value(0).toInt()
                                    << "left"       << query.value(4)
                                    << "right"      << query.value(5);
*/

        int split        = query.value(0).toInt();
        parentSplitAxis  = split;

        int embeddingId  = query.value(1).toInt();
        cv::Mat maxRange = cv::Mat(1, 128, CV_32F, query.value(2).toByteArray().data()).clone();
        cv::Mat minRange = cv::Mat(1, 128, CV_32F, query.value(3).toByteArray().data()).clone();
        QVariant left    = query.value(4);
        QVariant right   = query.value(5);

        bindingValues.clear();
        bindingValues << embeddingId;

        query = d->db->execQuery(QLatin1String("SELECT embedding FROM FaceMatrices WHERE id = ?"),
                                 bindingValues);

        if (! query.next())
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "fail to find parent face embedding" << query.lastError();
            return -1;
        }

        cv::Mat position = cv::Mat(1, 128, CV_32F, query.value(0).toByteArray().data()).clone();

        if (nodePos.at<float>(0, split) >= position.at<float>(0, split))
        {
            currentNode = right;
            leftChild   = false;
        }
        else
        {
            currentNode = left;
            leftChild   = true;
        }

         updateRangeTreeDb(parent, minRange, maxRange, nodePos);
    }

    return parent;
}

double FaceDb::getClosestNeighborsTreeDb(const DataNode& subTree,
                                         QMap<double, QVector<int> >& neighborList,
                                         const cv::Mat& position,
                                         float sqRange,
                                         float cosThreshold,
                                         int maxNbNeighbors) const
{
    if (subTree.isNull())
    {
        return sqRange;
    }

    // try to add current node to the list

    const float sqrdistanceToCurrentNode = KDNode::sqrDistance(position.ptr<float>(), subTree.position.ptr<float>(), 128);

    if ((sqrdistanceToCurrentNode < sqRange) &&
        (KDNode::cosDistance(position.ptr<float>(), subTree.position.ptr<float>(), 128) > cosThreshold))
    {
        neighborList[sqrdistanceToCurrentNode].append(subTree.label);

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

    double sqrDistanceLeftTree  = 0.0;
    double sqrDistanceRightTree = 0.0;
    DataNode leftNode;
    DataNode rightNode;

    if (subTree.left <= 0)
    {
        sqrDistanceLeftTree = DBL_MAX;
    }
    else
    {
        QVariantList bindingValues;
        bindingValues << subTree.left;

        DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT position, max_range, min_range, `left`, `right` "
                                                                "FROM KDTree WHERE id = ?"),
                                                  bindingValues);

        if (query.next())
        {
            // encapsulate data node

            leftNode.nodeID   = subTree.left;
            int embeddingID   = query.value(0).toInt();
            leftNode.maxRange = cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone();
            leftNode.minRange = cv::Mat(1, 128, CV_32F, query.value(2).toByteArray().data()).clone();
            leftNode.left     = query.value(3).toInt();
            leftNode.right    = query.value(4).toInt();

            bindingValues.clear();
            bindingValues << embeddingID;

            query = d->db->execQuery(QLatin1String("SELECT identity, embedding FROM FaceMatrices WHERE id = ?"),
                                     bindingValues);

            if (query.next())
            {
                leftNode.label        = query.value(0).toInt();
                leftNode.position     = cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone();

                const float* minRange = leftNode.minRange.ptr<float>();
                const float* maxRange = leftNode.maxRange.ptr<float>();
                const float* pos      = leftNode.position.ptr<float>();

                for (int i = 0 ; i < 128 ; ++i)
                {
                    sqrDistanceLeftTree += (pow(qMax((minRange[i] - pos[i]),      0.0f), 2) +
                                            pow(qMax((pos[i]      - maxRange[i]), 0.0f), 2));
                }
            }
        }
    }

    if (subTree.right <= 0)
    {
        sqrDistanceRightTree = DBL_MAX;
    }
    else
    {
        QVariantList bindingValues;
        bindingValues << subTree.right;

        DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT position, max_range, min_range, `left`, `right` "
                                                                "FROM KDTree WHERE id = ?"),
                                                  bindingValues);

        if (query.next())
        {
            // encapsulate data node

            rightNode.nodeID   = subTree.right;
            int embeddingID    = query.value(0).toInt();
            rightNode.maxRange = cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone();
            rightNode.minRange = cv::Mat(1, 128, CV_32F, query.value(2).toByteArray().data()).clone();
            rightNode.left     = query.value(3).toInt();
            rightNode.right    = query.value(4).toInt();

            bindingValues.clear();
            bindingValues << embeddingID;

            query = d->db->execQuery(QLatin1String("SELECT identity, embedding FROM FaceMatrices WHERE id = ?"),
                                     bindingValues);

            if (query.next())
            {
                rightNode.label             = query.value(0).toInt();
                rightNode.position          = cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone();

                const float* const minRange = rightNode.minRange.ptr<float>();
                const float* const maxRange = rightNode.maxRange.ptr<float>();
                const float* const pos      = rightNode.position.ptr<float>();

                for (int i = 0 ; i < 128 ; ++i)
                {
                    sqrDistanceRightTree += (pow(qMax((minRange[i] - pos[i]),      0.0f), 2) +
                                             pow(qMax((pos[i]      - maxRange[i]), 0.0f), 2));
                }
            }
        }
    }

    // traverse the closest area

    if (sqrDistanceLeftTree < sqrDistanceRightTree)
    {
        if (sqrDistanceLeftTree < sqRange)
        {
            // traverse left Tree

            sqRange = getClosestNeighborsTreeDb(leftNode, neighborList, position, sqRange, cosThreshold, maxNbNeighbors);

            if (sqrDistanceRightTree < sqRange)
            {
                // traverse right Tree

                sqRange = getClosestNeighborsTreeDb(rightNode, neighborList, position, sqRange, cosThreshold, maxNbNeighbors);
            }
        }
    }
    else
    {
        if (sqrDistanceRightTree < sqRange)
        {
            // traverse right Tree

            sqRange = getClosestNeighborsTreeDb(rightNode, neighborList, position, sqRange, cosThreshold, maxNbNeighbors);

            if (sqrDistanceLeftTree < sqRange)
            {
                // traverse left Tree

                sqRange = getClosestNeighborsTreeDb(leftNode, neighborList, position, sqRange, cosThreshold, maxNbNeighbors);
            }
        }
    }

    return sqRange;
}

} // namespace Digikam
