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

void FaceDb::updateDNNFaceModel(DNNFaceModel& model)
{
    QList<DNNFaceVecMetadata> metadataList = model.vecMetadata();

    for (size_t i = 0 ; i < (size_t)metadataList.size() ; ++i)
    {
        const DNNFaceVecMetadata& metadata = metadataList[i];

        if (metadata.storageStatus == DNNFaceVecMetadata::Created)
        {
            std::vector<float> vecdata = model.vecData(i);

            if (vecdata.size() > 2)
            {
                qCDebug(DIGIKAM_FACEDB_LOG) << "vecdata: " << vecdata[vecdata.size()-2]
                                                           << vecdata[vecdata.size()-1];
            }

            QByteArray vec_byte(vecdata.size()*sizeof(float), 0);
            float* const fp = (float*)vec_byte.data();

            for (size_t k = 0 ; k < vecdata.size() ; ++k)
            {
                *(fp + k) = vecdata[k];
            }

            QByteArray compressed_vecdata = qCompress(vec_byte);

            if (compressed_vecdata.isEmpty())
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
                                << compressed_vecdata;

                QVariantList values;
                d->db->execSql(QLatin1String("SELECT id FROM FaceMatrices "
                                                 "WHERE identity=? AND `context`=?;"),
                               metadata.identity, metadata.context, &values);

                if (values.count() > 20)
                {
                    for (int j = 0 ; j < values.count() - 20 ; ++j)
                    {
                        qCDebug(DIGIKAM_FACEDB_LOG) << "Delete face vec data " << values.at(j).toInt()
                                                    << " for identity " << metadata.identity;

                        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices "
                                                     "WHERE id=? AND identity=? AND `context`=?;"),
                                       values.at(j).toInt(), metadata.identity, metadata.context);
                    }
                }

                d->db->execSql(QLatin1String("INSERT INTO FaceMatrices (identity, `context`, vecdata) "
                                             "VALUES (?,?,?);"),
                               histogramValues, nullptr, &insertedId);

                model.setWrittenToDatabase(i, insertedId.toInt());

                qCDebug(DIGIKAM_FACEDB_LOG) << "Commit compressed vecData " << insertedId << " for identity "
                                            << metadata.identity << " with size " << compressed_vecdata.size();
            }
        }
    }
}

DNNFaceModel FaceDb::dnnFaceModel(bool debug) const
{
    if(debug)
    {
        return DNNFaceModel();
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "Loading DNN model";
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT id, identity, `context`, vecdata "
                                                            "FROM FaceMatrices;"));

    DNNFaceModel model = DNNFaceModel();
    QList<std::vector<float>> mats;
    QList<DNNFaceVecMetadata> matMetadata;

    while (query.next())
    {
        DNNFaceVecMetadata metadata;
        std::vector<float> vecdata;

        metadata.databaseId    = query.value(0).toInt();
        metadata.identity      = query.value(1).toInt();
        metadata.context       = query.value(2).toString();
        metadata.storageStatus = DNNFaceVecMetadata::InDatabase;
        QByteArray cData       = query.value(3).toByteArray();

        if (!cData.isEmpty())
        {
            QByteArray new_vec = qUncompress(cData);

            if (new_vec.isEmpty())
            {
                qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot uncompress mat data to checkout from database for Identity " << metadata.identity;
            }
            else
            {
                qCDebug(DIGIKAM_FACEDB_LOG) << "Checkout compressed histogram " << metadata.databaseId << " for identity "
                                            << metadata.identity << " with size " << cData.size();

                float* const it = (float *)new_vec.data();

                for (size_t i = 0 ; i < new_vec.size() / sizeof(float) ; ++i)
                {
                    vecdata.push_back(*(it+i));
                }

                mats        << vecdata;
                matMetadata << metadata;
            }
        }
        else
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Mat data to checkout from database are empty for Identity " << metadata.identity;
        }
    }

    model.setMats(mats, matMetadata);

    return model;
}

} // namespace Digikam
