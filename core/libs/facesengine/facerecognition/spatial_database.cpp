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

double sqrDistance1(std::vector<float> pos1, std::vector<float> pos2)
{
    Q_ASSERT(pos1.size() == pos2.size());

    double sqrDistance = 0;

    for (size_t i = 0; i < pos1.size(); ++i)
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
                                                                                    "label INTEGER NOT NULL,"
                                                                                    "split_axis INTEGER NOT NULL, "
                                                                                    "position TEXT NOT NULL, "
                                                                                    "max_range TEXT NOT NULL, "
                                                                                    "min_range TEXT NOT NULL,"
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

bool SpatialDatabase::insert(const std::vector<float>& nodePos, const int label)
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
                       QString::fromLatin1(QJsonDocument(FaceExtractor::encodeVector(nodePos)).toJson(QJsonDocument::Compact)));
    d->query.bindValue(QLatin1String(":max_range"),
                       QString::fromLatin1(QJsonDocument(FaceExtractor::encodeVector(nodePos)).toJson(QJsonDocument::Compact)));
    d->query.bindValue(QLatin1String(":min_range"),
                       QString::fromLatin1(QJsonDocument(FaceExtractor::encodeVector(nodePos)).toJson(QJsonDocument::Compact)));

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
    //qDebug() << "inserted node" << nodePos << "at index" << newNode << "at" << (isLeftChild?"left":"right") << "of node" << parentID;

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


bool SpatialDatabase::updateRange(int nodeId, std::vector<float>& minRange, std::vector<float>& maxRange, const std::vector<float>& position)
{
    for (size_t i = 0; i < minRange.size(); i++)
    {
        maxRange[i] = std::max(maxRange[i], position[i]);
        minRange[i] = std::min(minRange[i], position[i]);
    }

    d->query.prepare(QLatin1String("UPDATE kd_tree SET max_range = :maxrange, min_range = :minrange WHERE node_id = :id"));

    QString::fromLatin1(QJsonDocument(FaceExtractor::encodeVector(minRange)).toJson(QJsonDocument::Compact));

    d->query.bindValue(QLatin1String(":maxrange"),
                       QString::fromLatin1(QJsonDocument(FaceExtractor::encodeVector(maxRange)).toJson(QJsonDocument::Compact)));
    d->query.bindValue(QLatin1String(":minrange"),
                       QString::fromLatin1(QJsonDocument(FaceExtractor::encodeVector(minRange)).toJson(QJsonDocument::Compact)));
    d->query.bindValue(QLatin1String(":id"), nodeId);

    if(! d->query.exec())
    {
        qWarning() << "fail to update range, error" << d->query.lastError();

        return false;
    }

    return true;
}

int SpatialDatabase::findParent(const std::vector<float>& nodePos,bool& leftChild, int& parentSplitAxis)
{
    int parent = 1;
    QVariant currentNode = parent;

    while (currentNode.isValid() && ! currentNode.isNull())
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
                qDebug() << "Add root";
                return 0;
            }

            // error
            qWarning() << "Error query parent =" << parent << d->query.lastError();
            return -1;
        }

        int split = d->query.value(0).toInt();
/*
        qDebug() << "split axis" << d->query.value(0).toInt()
                 << "position"   << d->query.value(1).toString()
                 << "maxRange"   << d->query.value(2).toString()
                 << "minRange"   << d->query.value(3).toString()
                 << "left"       << d->query.value(4)
                 << "right"      << d->query.value(5);
*/
        parentSplitAxis = split;

        std::vector<float> position = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(1).toByteArray()).array());
        std::vector<float> maxRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(2).toByteArray()).array());
        std::vector<float> minRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(3).toByteArray()).array());

        if (nodePos[split] >= position[split])
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

double SpatialDatabase::getClosestNeighbors(const DataNode& subTree,
                                            QMap<double, QVector<int> >& neighborList,
                                            std::vector<float> position,
                                            double sqRange,
                                            int maxNbNeighbors)
{
    if (subTree.isNull())
    {
        return sqRange;
    }

    // add current node to the list
    const double sqrdistanceToCurrentNode = sqrDistance1(position, subTree.position);
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
            leftNode.position = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(1).toByteArray()).array());
            leftNode.maxRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(2).toByteArray()).array());
            leftNode.minRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(3).toByteArray()).array());
            leftNode.left     = d->query.value(4).toInt();
            leftNode.right    = d->query.value(5).toInt();

            for (size_t i = 0; i < leftNode.minRange.size(); ++i)
            {
                sqrDistanceleftTree += (pow(qMax((leftNode.minRange[i] - position[i]), 0.0f), 2) +
                                        pow(qMax((position[i] - leftNode.maxRange[i]), 0.0f), 2));
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
            rightNode.position = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(1).toByteArray()).array());
            rightNode.maxRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(2).toByteArray()).array());
            rightNode.minRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(3).toByteArray()).array());
            rightNode.left     = d->query.value(4).toInt();
            rightNode.right    = d->query.value(5).toInt();

            for (size_t i = 0; i < rightNode.minRange.size(); ++i)
            {
                sqrDistancerightTree += (pow(qMax((rightNode.minRange[i] - position[i]), 0.0f), 2) +
                                         pow(qMax((position[i] - rightNode.maxRange[i]), 0.0f), 2));
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
