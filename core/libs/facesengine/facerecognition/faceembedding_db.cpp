/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-07-13
 * Description : Implementation of 1NF database for face embedding storage
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

#include "faceembedding_db.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace RecognitionTest
{
class Q_DECL_HIDDEN FaceEmbeddingDb::Private
{
public:
    Private()
        : db(QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("face_embedding"))),
          query(db)
    {
        if (!db.isValid())
        {
            qFatal("face_embedding database doesn't have a valid driver");
        }

        db.setDatabaseName(QLatin1String("localface.db"));
        if (!db.open())
        {
            qFatal("Unable to establish a database connection");
        }


        query.exec(QLatin1String("SET sql_notes = 0"));
        bool success = query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS face_embedding (label INTEGER NOT NULL REFERENCES identity,"
                                                                                            "embedding BLOB NOT NULL)"));

        if (!success)
        {
            qWarning() << "fail to create face_embedding database" << query.lastError();
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

FaceEmbeddingDb::FaceEmbeddingDb()
    : d(new Private())
{
}

FaceEmbeddingDb::~FaceEmbeddingDb()
{
    delete d;
}

bool FaceEmbeddingDb::insert(const cv::Mat& faceEmbedding, const int label) const
{
    d->query.prepare(QLatin1String("INSERT INTO face_embedding (label, embedding) "
                                   "VALUES (:label, :embedding)"));

    d->query.bindValue(QLatin1String(":label"), label);
    d->query.bindValue(QLatin1String(":embedding"),
                       QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * 128)));

    if (!d->query.exec())
    {
        qWarning() << "fail to registered new face embedding, error" << d->query.lastError();

        return false;
    }

    return true;
}

KDTree FaceEmbeddingDb::reconstructTree() const
{
    KDTree tree(128);

    // favor new data
    if (d->query.exec(QLatin1String("SELECT label, embedding FROM face_embedding ORDER BY label")))
    {
        if (d->query.last())
        {
            do
            {
                Identity id;
                id.setId(d->query.value(0).toInt());
                cv::Mat recordedFaceEmbedding = cv::Mat(1, 128, CV_32F, d->query.value(1).toByteArray().data()).clone();

                tree.add(recordedFaceEmbedding, id);
            }
            while(d->query.previous());
        }
    }

    return tree;
}

}
