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
        bool success = query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS face_embedding (id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                                                            "label INTEGER NOT NULL REFERENCES identity,"
                                                                                            "context TEXT,"
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

int FaceEmbeddingDb::insert(const cv::Mat& faceEmbedding, const int label, const QString& context) const
{
    d->query.prepare(QLatin1String("INSERT INTO face_embedding (label, context, embedding) "
                                   "VALUES (:label, :context, :embedding)"));

    d->query.bindValue(QLatin1String(":label"), label);
    d->query.bindValue(QLatin1String(":context"), context);
    d->query.bindValue(QLatin1String(":embedding"),
                       QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * 128)));

    if (!d->query.exec())
    {
        qWarning() << "fail to registered new face embedding, error" << d->query.lastError();

        return -1;
    }

    return d->query.lastInsertId().toInt();
}

KDTree* FaceEmbeddingDb::reconstructTree() const
{
    KDTree* tree = new KDTree(128);

    // favor new data
    if (d->query.exec(QLatin1String("SELECT id, label, embedding FROM face_embedding")))
    {
        if (d->query.last())
        {
            do
            {
                int nodeId = d->query.value(0).toInt();
                int identity = d->query.value(1).toInt();
                cv::Mat recordedFaceEmbedding = cv::Mat(1, 128, CV_32F, d->query.value(2).toByteArray().data()).clone();

                KDNode* newNode = tree->add(recordedFaceEmbedding, identity);

                if (newNode)
                {
                    newNode->setNodeId(nodeId);
                }
                else
                {
                    qWarning() << "Error insert node" << nodeId;
                }
            }
            while(d->query.previous());
        }
    }

    return tree;
}

cv::Ptr<cv::ml::TrainData> FaceEmbeddingDb::trainData() const
{
    cv::Mat feature, label;

    if (d->query.exec(QLatin1String("SELECT label, embedding FROM face_embedding")))
    {
        if (d->query.last())
        {
            do
            {
                label.push_back(d->query.value(0).toInt());
                feature.push_back(cv::Mat(1, 128, CV_32F, d->query.value(1).toByteArray().data()).clone());
            }
            while(d->query.previous());
        }
    }

    return cv::ml::TrainData::create(feature, 0, label);
}

}
