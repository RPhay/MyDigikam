/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train identities.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

void FaceDb::updateEIGENFaceModel(EigenFaceModel& model, const std::vector<cv::Mat>& images_rgb)
{
    QList<EigenFaceMatMetadata> metadataList = model.matMetadata();

    for (size_t i = 0, j = 0 ; i < (size_t)metadataList.size() ; ++i)
    {
        const EigenFaceMatMetadata& metadata = metadataList[i];

        if (metadata.storageStatus == EigenFaceMatMetadata::Created)
        {
            OpenCVMatData data = model.matData(i);
            cv::Mat mat_rgb;

            if (j >= images_rgb.size())
            {
                qCWarning(DIGIKAM_FACEDB_LOG) << "updateEIGENFaceModel: the size of images_rgb is wrong";
            }
            else
            {
                mat_rgb = images_rgb[j++];
            }

            if (data.data.isEmpty())
            {
                qCWarning(DIGIKAM_FACEDB_LOG) << "Eigenface data to commit in database are empty for Identity "
                                              << metadata.identity;
            }
            else
            {
                QByteArray compressed = qCompress(data.data);
                std::vector<float> vecdata;

                /**
                 * FIXME !!! Why the Eigen face use DNN code here ???
                 * Otherwise, how does it comput vecdata ???
                 * Buggy codes from GSoC 2017
                 */
/*
                this->getFaceVector(mat_rgb, vecdata);
*/
                QByteArray vec_byte(vecdata.size() * sizeof(float), 0);
                float* const fp = reinterpret_cast<float*>(vec_byte.data());

                for (size_t k = 0 ; k < vecdata.size() ; ++k)
                {
                    *(fp + k) = vecdata[k];
                }

                QByteArray compressed_vecdata = qCompress(vec_byte);

                if (compressed.isEmpty())
                {
                    qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot compress mat data to commit in database for Identity "
                                                  << metadata.identity;
                }
                else if (compressed_vecdata.isEmpty())
                {
                    qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot compress face vec data to commit in database for Identity "
                                                  << metadata.identity;
                }
                else
                {
                    QVariantList histogramValues;
                    QVariant     insertedId;

                    histogramValues << metadata.identity
                                    << metadata.context
                                    << data.type
                                    << data.rows
                                    << data.cols
                                    << compressed
                                    << compressed_vecdata;

                    d->db->execSql(QLatin1String("INSERT INTO FaceMatrices (identity, `context`, `type`, `rows`, `cols`, `data`, vecdata) "
                                                 "VALUES (?,?,?,?,?,?,?);"),
                                   histogramValues, nullptr, &insertedId);

                    model.setWrittenToDatabase(i, insertedId.toInt());

                    qCDebug(DIGIKAM_FACEDB_LOG) << "Commit compressed matData " << insertedId << " for identity "
                                                << metadata.identity << " with size " << compressed.size();
                }
            }
        }
    }
}

EigenFaceModel FaceDb::eigenFaceModel() const
{
    qCDebug(DIGIKAM_FACEDB_LOG) << "Loading EIGEN model";
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT id, identity, `context`, `type`, `rows`, `cols`, `data`, vecdata "
                                                            "FROM FaceMatrices;"));

    EigenFaceModel model = EigenFaceModel();
    QList<OpenCVMatData>        mats;
    QList<EigenFaceMatMetadata> matMetadata;

    while (query.next())
    {
        EigenFaceMatMetadata metadata;
        OpenCVMatData data;

        metadata.databaseId    = query.value(0).toInt();
        metadata.identity      = query.value(1).toInt();
        metadata.context       = query.value(2).toString();
        metadata.storageStatus = EigenFaceMatMetadata::InDatabase;

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
                qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot uncompress mat data to checkout from database for Identity "
                                              << metadata.identity;
            }
            else
            {
                qCDebug(DIGIKAM_FACEDB_LOG) << "Checkout compressed histogram " << metadata.databaseId
                                            << " for identity " << metadata.identity << " with size " << cData.size();

                mats        << data;
                matMetadata << metadata;
            }
        }
        else
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Mat data to checkout from database are empty for Identity "
                                          << metadata.identity;
        }
    }

    model.setMats(mats, matMetadata);

    return model;
}

void FaceDb::clearEIGENTraining(const QString& context)
{
    if (context.isNull())
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices;"));
    }
    else
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE `context`=?;"),
                       context);
    }
}

void FaceDb::clearEIGENTraining(const QList<int>& identities, const QString& context)
{
    foreach (int id, identities)
    {
        if (context.isNull())
        {
            d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=?;"),
                           id);
        }
        else
        {
            d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=? AND `context`=?;"),
                           id, context);
        }
    }
}

} // namespace Digikam
