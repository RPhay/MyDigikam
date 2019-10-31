/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train identities.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

namespace
{
    enum
    {
        LBPHStorageVersion = 1
    };
}

void FaceDb::updateLBPHFaceModel(LBPHFaceModel& model)
{
    QVariantList values;
    values << LBPHStorageVersion << model.radius() << model.neighbors() << model.gridX() << model.gridY();

    if (model.databaseId)
    {
        values << model.databaseId;
        d->db->execSql(QLatin1String("UPDATE OpenCVLBPHRecognizer SET version=?, radius=?, neighbors=?, grid_x=?, grid_y=? WHERE id=?;"), values);
    }
    else
    {
        QVariant insertedId;
        d->db->execSql(QLatin1String("INSERT INTO OpenCVLBPHRecognizer (version, radius, neighbors, grid_x, grid_y) VALUES (?,?,?,?,?);"),
                       values, nullptr, &insertedId);
        model.databaseId = insertedId.toInt();
    }

    QList<LBPHistogramMetadata> metadataList = model.histogramMetadata();

    for (int i = 0 ; i < metadataList.size() ; ++i)
    {
        const LBPHistogramMetadata& metadata = metadataList[i];

        if (metadata.storageStatus == LBPHistogramMetadata::Created)
        {
            OpenCVMatData data = model.histogramData(i);

            if (data.data.isEmpty())
            {
                qCWarning(DIGIKAM_FACEDB_LOG) << "Histogram data to commit in database are empty for Identity "
                                              << metadata.identity;
            }
            else
            {
                QByteArray compressed = qCompress(data.data);

                if (compressed.isEmpty())
                {
                    qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot compress histogram data to commit in database for Identity "
                                                  << metadata.identity;
                }
                else
                {
                    QVariantList histogramValues;
                    QVariant     insertedId;

                    histogramValues << model.databaseId
                                    << metadata.identity
                                    << metadata.context
                                    << data.type
                                    << data.rows
                                    << data.cols
                                    << compressed;

                    values.clear();
                    d->db->execSql(QLatin1String("SELECT id FROM OpenCVLBPHistograms "
                                                 "WHERE recognizerid=? AND identity=? AND `type`=?;"),
                                   model.databaseId, metadata.identity, data.type, &values);

                    if (values.count() > 20)
                    {
                        for (int j = 0 ; j < values.count() - 20 ; ++j)
                        {
                            qCDebug(DIGIKAM_FACEDB_LOG) << "Delete compressed histogram " << values.at(j).toInt()
                                                        << " for identity " << metadata.identity;

                            d->db->execSql(QLatin1String("DELETE FROM OpenCVLBPHistograms "
                                                         "WHERE id=? AND recognizerid=? AND identity=? AND `type`=?;"),
                                           values.at(j).toInt(), model.databaseId, metadata.identity, data.type);
                        }
                    }

                    d->db->execSql(QLatin1String("INSERT INTO OpenCVLBPHistograms (recognizerid, identity, `context`, `type`, `rows`, `cols`, `data`) "
                                                 "VALUES (?,?,?,?,?,?,?);"),
                                   histogramValues, nullptr, &insertedId);

                    model.setWrittenToDatabase(i, insertedId.toInt());

                    qCDebug(DIGIKAM_FACEDB_LOG) << "Commit compressed histogram " << insertedId.toInt() << " for identity "
                                                << metadata.identity << " with size " << compressed.size();
                }
            }
        }
    }
}

LBPHFaceModel FaceDb::lbphFaceModel() const
{
    QVariantList values;
    qCDebug(DIGIKAM_FACEDB_LOG) << "Loading LBPH model";
    d->db->execSql(QLatin1String("SELECT id, version, radius, neighbors, grid_x, grid_y FROM OpenCVLBPHRecognizer;"), &values);

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        LBPHFaceModel model;
        model.databaseId = it->toInt();
        ++it;

        qCDebug(DIGIKAM_FACEDB_LOG) << "Found model id" << model.databaseId;

        int version      = it->toInt();
        ++it;

        if (version > LBPHStorageVersion)
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Unsupported LBPH storage version" << version;
            it += 4;
            continue;
        }

        model.setRadius(it->toInt());
        ++it;
        model.setNeighbors(it->toInt());
        ++it;
        model.setGridX(it->toInt());
        ++it;
        model.setGridY(it->toInt());
        ++it;

        DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT id, identity, `context`, `type`, `rows`, `cols`, `data` "
                                                                "FROM OpenCVLBPHistograms WHERE recognizerid=?;"),
                                                  model.databaseId);
        QList<OpenCVMatData> histograms;
        QList<LBPHistogramMetadata> histogramMetadata;

        while (query.next())
        {
            LBPHistogramMetadata metadata;
            OpenCVMatData        data;

            metadata.databaseId    = query.value(0).toInt();
            metadata.identity      = query.value(1).toInt();
            metadata.context       = query.value(2).toString();
            metadata.storageStatus = LBPHistogramMetadata::InDatabase;

            // cv::Mat
            data.type              = query.value(3).toInt();
            data.rows              = query.value(4).toInt();
            data.cols              = query.value(5).toInt();
            QByteArray cData       = query.value(6).toByteArray();

            if (!cData.isEmpty())
            {
                data.data = qUncompress(cData);

                if (data.data.isEmpty())
                {
                    qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot uncompress histogram data to checkout from database for Identity "
                                                  << metadata.identity;
                }
                else
                {
                    qCDebug(DIGIKAM_FACEDB_LOG) << "Checkout compressed histogram " << metadata.databaseId << " for identity "
                                                << metadata.identity << " with size " << cData.size();

                    histograms        << data;
                    histogramMetadata << metadata;
                }
            }
            else
            {
                qCWarning(DIGIKAM_FACEDB_LOG) << "Histogram data to checkout from database are empty for Identity "
                                              << metadata.identity;
            }
        }

        model.setHistograms(histograms, histogramMetadata);

        return model;
    }

    return LBPHFaceModel();
}

void FaceDb::clearLBPHTraining(const QString& context)
{
    if (context.isNull())
    {
        d->db->execSql(QLatin1String("DELETE FROM OpenCVLBPHistograms;"));
        d->db->execSql(QLatin1String("DELETE FROM OpenCVLBPHRecognizer;"));
    }
    else
    {
        d->db->execSql(QLatin1String("DELETE FROM OpenCVLBPHistograms WHERE `context`=?;"), context);
    }
}

void FaceDb::clearLBPHTraining(const QList<int>& identities, const QString& context)
{
    foreach (int id, identities)
    {
        if (context.isNull())
        {
            d->db->execSql(QLatin1String("DELETE FROM OpenCVLBPHistograms WHERE identity=?;"), id);
        }
        else
        {
            d->db->execSql(QLatin1String("DELETE FROM OpenCVLBPHistograms WHERE identity=? AND `context`=?;"), id, context);
        }
    }
}

} // namespace Digikam
