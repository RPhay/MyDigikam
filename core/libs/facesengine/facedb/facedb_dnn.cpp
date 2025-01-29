/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train face recognizer.
 *
 * SPDX-FileCopyrightText: 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facedb_p.h"

// Local includes

namespace Digikam
{

int FaceDb::insertFaceVector(const cv::Mat& faceEmbedding,
                             const int label,
                             const QString& hash) const
{
    QVariantList bindingValues;

    bindingValues << label;
    bindingValues << hash;
    bindingValues << QByteArray::fromRawData(reinterpret_cast<const char*>(faceEmbedding.ptr<float>()), (sizeof(float) * 128));

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

void FaceDb::getTrainingVersionInfo(QString& version, QString& model) const
{
    version = setting(QStringLiteral("TrainingVersion"));
    model = setting(QStringLiteral("ExtractorModel"));
}

void FaceDb::setTrainingVersionInfo(const QString& version, const QString& model)
{
    setSetting(QStringLiteral("TrainingVersion"), version);
    setSetting(QStringLiteral("ExtractorModel"), model);
}

} // namespace Digikam
