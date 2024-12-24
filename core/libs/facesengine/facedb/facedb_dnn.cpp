/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train face recognizer.
 *
 * SPDX-FileCopyrightText: 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facedb_p.h"

// Local includes

#include "kd_treeopenface.h"
#include "kd_treesface.h"

namespace Digikam
{

int FaceDb::insertFaceVector(const cv::Mat& faceEmbedding,
                             const int label,
                             const QString& hash) const
{
    QVariantList bindingValues;

    bindingValues << label;
    bindingValues << hash;
    bindingValues << QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * 128));

    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("INSERT INTO FaceMatrices (identity, removeHash, embedding) "
                                                            "VALUES (?,?,?);"),
                                              bindingValues);

    if (query.lastInsertId().isNull())
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "fail to insert face embedding, last query"
                                      << query.lastQuery()
                                      << "bound values" << query.boundValues()
                                      << query.lastError();
    }
    else
    {
        qCDebug(DIGIKAM_FACEDB_LOG) << "Commit face mat data "
                                    << query.lastInsertId().toInt()
                                    << " for identity " << label;
    }

    return query.lastInsertId().toInt();
}

bool FaceDb::removeFaceVector(const int nodeId) const
{
    d->db->execQuery(QLatin1String("DELETE FROM FaceMatrices WHERE id=?;"),
                                            nodeId);
    qCDebug(DIGIKAM_FACEDB_LOG) << "Deleted nodeId " << nodeId << " from FaceMatricies";

    return true;
}

bool FaceDb::removeFaceVector(const QString& hash) const
{
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("DELETE FROM FaceMatrices WHERE removeHash=?;"), hash);        

    qCDebug(DIGIKAM_FACEDB_LOG) << "Deleted hash " << hash << " from FaceMatricies";

    // TODO: Check query result.

    return true;
}

// KDTreeBase* FaceDb::reconstructTree(FaceScanSettings::FaceRecognitionModel recModel)
// {
//     KDTreeBase* tree  = nullptr;
//     d->recognizeModel = recModel;

//     switch (d->recognizeModel)
//     {
//         case FaceScanSettings::FaceRecognitionModel::OpenFace:
//         {
//             tree     = new KDTreeOpenFace(128);
//             break;
//         }

//         case FaceScanSettings::FaceRecognitionModel::SFace:
//         {
//             tree     = new KDTreeSFace(128);
//             break;
//         }

//         default:
//         {
//              qCritical(DIGIKAM_DPLUGIN_GENERIC_LOG) << "FaceDb::reconstructTree Unknown recognition model specified" << Qt::endl;
//         }
//     }

//     DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT id, identity, embedding FROM FaceMatrices;"));

//     while (query.next())
//     {
//         int nodeId                    = query.value(0).toInt();
//         int identity                  = query.value(1).toInt();
//         cv::Mat recordedFaceEmbedding = cv::Mat(1, 128, CV_32F, query.value(2).toByteArray().data()).clone();
//         KDNodeBase* const newNode     = tree->add(recordedFaceEmbedding, identity);

//         if (newNode)
//         {
//             newNode->setNodeId(nodeId);
//         }
//         else
//         {
//             qCWarning(DIGIKAM_FACEDB_LOG) << "Error insert node" << nodeId;
//         }
//     }

//     return tree;
// }

cv::Ptr<cv::ml::TrainData> FaceDb::trainData() const
{
    cv::Mat feature, label;
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT identity, embedding "
                                                            "FROM FaceMatrices;"));

    while (query.next())
    {
        label.push_back(query.value(0).toInt());
        feature.push_back(cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone());
    }

    if (0 == feature.rows)
    {
        return nullptr;
    }
    else
    {
        return cv::ml::TrainData::create(feature, cv::ml::ROW_SAMPLE, label);
    }
}

void FaceDb::clearDNNTraining()
{
    d->db->execSql(QLatin1String("DELETE FROM FaceMatrices;"));
}

void FaceDb::clearDNNTraining(const QList<int>& identities)
{
    for (int id : std::as_const(identities))
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=?;"),
                       id);
    }
}

} // namespace Digikam
