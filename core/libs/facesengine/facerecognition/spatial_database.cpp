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
#include <QDebug>

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
                                                                                    "left INTEGER NOT NULL REFERENCES kd_tree,"
                                                                                    "right INTEGER NOT NULL REFERENCES kd_tree)"));

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

bool SpatialDatabase::updateRange(int nodeId, std::vector<float> minRange, std::vector<float> maxRange, std::vector<float> position)
{
    for (size_t i = 0; i < minRange.size(); i++)
    {
        maxRange[i] = std::max(maxRange[i], position[i]);
        minRange[i] = std::min(minRange[i], position[i]);
    }

    return true;
}


}
