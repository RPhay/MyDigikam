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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QDebug>

#include "faceextractor.h"

namespace RecognitionTest
{
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
                                                                                    "parent INTEGER NOT NULL REFERENCES kd_tree,"
                                                                                    "left INTEGER REFERENCES kd_tree,"
                                                                                    "right INTEGER REFERENCES kd_tree)"));

        if (!success)
        {
            qDebug() << "fail to create kd_tree database" << query.lastError();
        }

        if (! query.exec(QLatin1String("CREATE UNIQUE INDEX idx_node ON kd_tree (node_id)")))
        {
            qDebug() << "fail to create index on kd_tree database" << query.lastError();
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
        qDebug() << "fail to update range, error" << d->query.lastError();

        return false;
    }

    return true;
}

int SpatialDatabase::findParent(const std::vector<float>& nodePos)
{
    int parent = 1;
    QVariant currentNode = parent;

    d->query.prepare(QLatin1String("SELECT split_axis, position, max_range, min_range, left, right FROM kd_tree WHERE node_id = :id"));

    while (currentNode.isValid() && ! currentNode.isNull())
    {
        parent = currentNode.toInt();
        d->query.bindValue(QLatin1String(":id"), parent);

        if(! d->query.exec())
        {
            qDebug() << "fail to query node, error" << d->query.lastError();
        }

        if (d->query.size() == 0)
        {
            if (parent == 1)
            {
                // add root
                return 0;
            }
            else
            {
                // error
                return -1;
            }
        }

        int split = d->query.value(0).toInt();

        std::vector<float> position = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(1).toByteArray()).array());
        std::vector<float> maxRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(2).toByteArray()).array());
        std::vector<float> minRange = FaceExtractor::decodeVector(QJsonDocument::fromJson(d->query.value(3).toByteArray()).array());

        updateRange(parent, minRange, maxRange, nodePos);

        if (nodePos[split] >= position[split])
        {
            currentNode = d->query.value(5).toInt();
        }
        else
        {
            currentNode = d->query.value(4).toInt();
        }
    }

    return parent;
}

}
