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

#include "spatial_database.h"

// std include
#include <cfloat>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QDebug>

#include "faceextractor.h"

namespace RecognitionTest
{

double sqrDistance1(const float* pos1, const float* pos2, int dimension)
{
    double sqrDistance = 0;

    for (int i = 0; i < dimension; ++i)
    {
        sqrDistance += pow((pos1[i] - pos2[i]), 2);
    }

    return sqrDistance;
}

class Q_DECL_HIDDEN SpatialDatabase::Private
{
public:
    Private()
        : db(QSqlDatabase::addDatabase(QLatin1String("QSQLITE")))
    {
        db.setDatabaseName(QLatin1String("localface.db"));
        if (!db.open())
        {
            qFatal("Unable to establish a database connection");
        }


        query.exec(QLatin1String("SET sql_notes = 0"));
        bool success = query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS kd_tree (node_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                                    "label INTEGER NOT NULL REFERENCES identity,"
                                                                                    "split_axis INTEGER NOT NULL, "
                                                                                    "position BLOB NOT NULL, "
                                                                                    "max_range BLOB NOT NULL, "
                                                                                    "min_range BLOB NOT NULL,"
                                                                                    "parent INTEGER REFERENCES kd_tree,"
                                                                                    "left INTEGER REFERENCES kd_tree,"
                                                                                    "right INTEGER REFERENCES kd_tree)"));

        if (!success)
        {
            qWarning() << "fail to create kd_tree database" << query.lastError();
        }

        if (! query.exec(QLatin1String("CREATE UNIQUE INDEX IF NOT EXISTS idx_node ON kd_tree (node_id)")))
        {
            qWarning() << "fail to create index on kd_tree database" << query.lastError();
        }

        query.exec(QLatin1String("SET sql_notes = 1"));
    }

    ~Private()
    {
        db.close();
    }

public:

    QSqlDatabase db;
    QSqlQuery    query;
};

SpatialDatabase::SpatialDatabase()
    : d(new Private())
{
}

SpatialDatabase::~SpatialDatabase()
{
    delete d;
}

bool SpatialDatabase::insert(const cv::Mat& nodePos, const int label) const
{
    bool isLeftChild = false;
    int parentSplitAxis = 0;

    int parentID = findParent(nodePos, isLeftChild, parentSplitAxis);

    if (parentID < 0)
    {
        return false;
    }

    // insert node to database
    d->query.prepare(QLatin1String("INSERT INTO kd_tree (label, split_axis, position, max_range, min_range, parent, left, right) "
                                   "VALUES (:label, :split_axis, :position, :max_range, :min_range, :parent, NULL, NULL)"));

    d->query.bindValue(QLatin1String(":label"), label);
    d->query.bindValue(QLatin1String(":split_axis"), (parentSplitAxis + 1) % 128);
    d->query.bindValue(QLatin1String(":position"),
                       QByteArray::fromRawData((char*)nodePos.ptr<float>(), (sizeof(float) * 128)));
    d->query.bindValue(QLatin1String(":max_range"),
                       QByteArray::fromRawData((char*)nodePos.ptr<float>(), (sizeof(float) * 128)));
    d->query.bindValue(QLatin1String(":min_range"),
                       QByteArray::fromRawData((char*)nodePos.ptr<float>(), (sizeof(float) * 128)));

    if (parentID > 0)
    {
        d->query.bindValue(QLatin1String(":parent"), parentID);
    }

    if (!d->query.exec())
    {
        qWarning() << "fail to registered new node, error" << d->query.lastError();

        return false;
    }

    int newNode = d->query.lastInsertId().toInt();

    if (parentID > 0)
    {
        // not root -> update parent
        if (isLeftChild)
        {
            d->query.prepare(QLatin1String("UPDATE kd_tree SET left = :child WHERE node_id = :id"));
        }
        else
        {
            d->query.prepare(QLatin1String("UPDATE kd_tree SET right = :child WHERE node_id = :id"));
        }

        d->query.bindValue(QLatin1String(":child"), newNode);
        d->query.bindValue(QLatin1String(":id"), parentID);

        if (!d->query.exec())
        {
            qWarning() << "fail to update child to parent, error" << d->query.lastError();
        }
    }

    return true;
}


bool SpatialDatabase::updateRange(int nodeId, cv::Mat& minRange, cv::Mat& maxRange, const cv::Mat& position) const
{
    float* min = minRange.ptr<float>();
    float* max = maxRange.ptr<float>();
    const float* pos = position.ptr<float>();

    for (int i = 0; i < position.cols; ++i)
    {
        max[i] = std::max(max[i], pos[i]);
        min[i] = std::min(min[i], pos[i]);
    }

    d->query.prepare(QLatin1String("UPDATE kd_tree SET max_range = :maxrange, min_range = :minrange WHERE node_id = :id"));

    d->query.bindValue(QLatin1String(":maxrange"),
                       QByteArray::fromRawData((char*)max, (sizeof(float) * 128)));
    d->query.bindValue(QLatin1String(":minrange"),
                      QByteArray::fromRawData((char*)min, (sizeof(float) * 128)));
    d->query.bindValue(QLatin1String(":id"), nodeId);

    if(! d->query.exec())
    {
        qWarning() << "fail to update range, error" << d->query.lastError();

        return false;
    }

    return true;
}

int SpatialDatabase::findParent(const cv::Mat& nodePos,bool& leftChild, int& parentSplitAxis) const
{
    int parent = 1;
    QVariant currentNode = parent;

    while (currentNode.isValid() && !currentNode.isNull())
    {
        parent = currentNode.toInt();
        d->query.prepare(QLatin1String("SELECT split_axis, position, max_range, min_range, left, right FROM kd_tree WHERE node_id = :id"));
        d->query.bindValue(QLatin1String(":id"), parent);

        if(! d->query.exec())
        {
            qWarning() << "fail to query node, error" << d->query.lastError();

            return -1;
        }

        if (! d->query.last())
        {
            if (parent == 1)
            {
                // add root
                return 0;
            }

            // error
            qWarning() << "Error query parent =" << parent << d->query.lastError();
            return -1;
        }

        int split = d->query.value(0).toInt();
/*
        qDebug() << "split axis" << d->query.value(0).toInt()
                 << "left"       << d->query.value(4)
                 << "right"      << d->query.value(5);
*/
        parentSplitAxis = split;

        cv::Mat position = cv::Mat(1, 128, CV_32F, d->query.value(1).toByteArray().data()).clone();
        cv::Mat maxRange = cv::Mat(1, 128, CV_32F, d->query.value(2).toByteArray().data()).clone();
        cv::Mat minRange = cv::Mat(1, 128, CV_32F, d->query.value(3).toByteArray().data()).clone();

        if (nodePos.at<float>(0, split) >= position.at<float>(0, split))
        {
            currentNode = d->query.value(5);
            leftChild = false;
        }
        else
        {
            currentNode = d->query.value(4);
            leftChild = true;
        }

        if(! updateRange(parent, minRange, maxRange, nodePos))
        {
            qWarning() << "fail to update range of node";
        }
    }

    return parent;
}

QMap<double, QVector<int> > SpatialDatabase::getClosestNeighbors(const cv::Mat& position,
                                                                 double sqRange,
                                                                 int maxNbNeighbors) const
{
    QMap<double, QVector<int> > closestNeighbors;

    DataNode root;
    d->query.prepare(QLatin1String("SELECT label, position, max_range, min_range, left, right FROM kd_tree WHERE node_id = :id"));
    d->query.bindValue(QLatin1String(":id"), 1);

    if(d->query.exec() && d->query.last())
    {
        // encapsulate data node
        root.nodeID   = 1;
        root.label    = d->query.value(0).toInt();
        root.position = cv::Mat(1, 128, CV_32F, d->query.value(1).toByteArray().data()).clone();
        root.maxRange = cv::Mat(1, 128, CV_32F, d->query.value(2).toByteArray().data()).clone();
        root.minRange = cv::Mat(1, 128, CV_32F, d->query.value(3).toByteArray().data()).clone();
        root.left     = d->query.value(4).toInt();
        root.right    = d->query.value(5).toInt();

        sqRange = getClosestNeighbors(root, closestNeighbors, position, sqRange, maxNbNeighbors);
    }

    return closestNeighbors;
}

double SpatialDatabase::getClosestNeighbors(const DataNode& subTree,
                                            QMap<double, QVector<int> >& neighborList,
                                            const cv::Mat& position,
                                            double sqRange,
                                            int maxNbNeighbors) const
{
    if (subTree.isNull())
    {
        return sqRange;
    }

    // add current node to the list
    const double sqrdistanceToCurrentNode = sqrDistance1(position.ptr<float>(), subTree.position.ptr<float>(), 128);

    if (sqrdistanceToCurrentNode < sqRange)
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
    double sqrDistanceleftTree  = 0;
    double sqrDistancerightTree = 0;
    DataNode leftNode;
    DataNode rightNode;

    if (subTree.left <= 0)
    {
        sqrDistanceleftTree = DBL_MAX;
    }
    else
    {
        d->query.prepare(QLatin1String("SELECT label, position, max_range, min_range, left, right FROM kd_tree WHERE node_id = :id"));
        d->query.bindValue(QLatin1String(":id"), subTree.left);

        if(d->query.exec() && d->query.last())
        {
            // encapsulate data node
            leftNode.nodeID   = subTree.left;
            leftNode.label    = d->query.value(0).toInt();
            leftNode.position = cv::Mat(1, 128, CV_32F, d->query.value(1).toByteArray().data()).clone();
            leftNode.maxRange = cv::Mat(1, 128, CV_32F, d->query.value(2).toByteArray().data()).clone();
            leftNode.minRange = cv::Mat(1, 128, CV_32F, d->query.value(3).toByteArray().data()).clone();
            leftNode.left     = d->query.value(4).toInt();
            leftNode.right    = d->query.value(5).toInt();

            const float* minRange = leftNode.minRange.ptr<float>();
            const float* maxRange = leftNode.maxRange.ptr<float>();
            const float* pos = position.ptr<float>();

            for (int i = 0; i < 128; ++i)
            {
                sqrDistanceleftTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                         pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
            }
        }
    }

    if (subTree.right <= 0)
    {
        sqrDistancerightTree = DBL_MAX;
    }
    else
    {
        d->query.prepare(QLatin1String("SELECT label, position, max_range, min_range, left, right FROM kd_tree WHERE node_id = :id"));
        d->query.bindValue(QLatin1String(":id"), subTree.right);

        if(d->query.exec() && d->query.last())
        {
            // encapsulate data node
            rightNode.nodeID   = subTree.right;
            rightNode.label    = d->query.value(0).toInt();
            rightNode.position = cv::Mat(1, 128, CV_32F, d->query.value(1).toByteArray().data()).clone();
            rightNode.maxRange = cv::Mat(1, 128, CV_32F, d->query.value(2).toByteArray().data()).clone();
            rightNode.minRange = cv::Mat(1, 128, CV_32F, d->query.value(3).toByteArray().data()).clone();
            rightNode.left     = d->query.value(4).toInt();
            rightNode.right    = d->query.value(5).toInt();

            const float* minRange = rightNode.minRange.ptr<float>();
            const float* maxRange = rightNode.maxRange.ptr<float>();
            const float* pos = position.ptr<float>();

            for (int i = 0; i < 128; ++i)
            {
                sqrDistancerightTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                         pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
            }
        }
    }

    // traverse the closest area
    if (sqrDistanceleftTree < sqrDistancerightTree)
    {
        if (sqrDistanceleftTree < sqRange)
        {
            // traverse left Tree
            sqRange = getClosestNeighbors(leftNode, neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistancerightTree < sqRange)
            {
                // traverse right Tree
                sqRange = getClosestNeighbors(rightNode, neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }
    else
    {
        if (sqrDistancerightTree < sqRange)
        {
            // traverse right Tree
            sqRange = getClosestNeighbors(rightNode, neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistanceleftTree < sqRange)
            {
                // traverse left Tree
                sqRange = getClosestNeighbors(leftNode, neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }

    return sqRange;
}

}
