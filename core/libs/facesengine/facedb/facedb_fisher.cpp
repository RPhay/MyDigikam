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

FisherFaceModel FaceDb::fisherFaceModel() const
{
    qCDebug(DIGIKAM_FACEDB_LOG) << "Loading FISHER model from FaceMatrices";
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT id, identity, `context`, `type`, `rows`, `cols`, `data`, vecdata "
                                                            "FROM FaceMatrices;"));

    FisherFaceModel model  = FisherFaceModel();
    QList<OpenCVMatData>         mats;
    QList<FisherFaceMatMetadata> matMetadata;

    while (query.next())
    {
        FisherFaceMatMetadata metadata;
        OpenCVMatData data;

        metadata.databaseId    = query.value(0).toInt();
        metadata.identity      = query.value(1).toInt();
        metadata.context       = query.value(2).toString();
        metadata.storageStatus = FisherFaceMatMetadata::InDatabase;

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
                qCDebug(DIGIKAM_FACEDB_LOG) << "Checkout compressed histogram " << metadata.databaseId << " for identity "
                                            << metadata.identity << " with size " << cData.size();

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

} // namespace Digikam
