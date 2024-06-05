/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train identities.
 *
 * SPDX-FileCopyrightText: 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_DB_H
#define DIGIKAM_FACE_DB_H

// C++ includes

#include <vector>

// Qt includes

#include <QFile>
#include <QString>
#include <QStandardPaths>

// Local includes

#include "digikam_config.h"
#include "digikam_opencv.h"
#include "identity.h"
#include "facedbbackend.h"

namespace Digikam
{

class KDTree;

class FaceDb
{
public:

    explicit FaceDb(FaceDbBackend* const db);
    ~FaceDb();

    BdEngineBackend::QueryState setSetting(const QString& keyword, const QString& value);
    QString setting(const QString& keyword)                                     const;

    /**
     * Returns true if the integrity of the database is preserved.
     */
    bool integrityCheck();

    /**
     * Shrinks the database.
     */
    void vacuum();

public:

    // --- Identity management (facedb_identity.cpp)

    int  addIdentity()                                                          const;
    int  getNumberOfIdentities()                                                const;

    void updateIdentity(const Identity& p);
    void deleteIdentity(int id);
    void deleteIdentity(const QString& uuid);
    void clearIdentities();

    QList<Identity> identities()                                                const;
    QList<int>      identityIds()                                               const;

public:

    // --- OpenCV DNN

    /**
     * @brief insertFaceVector : insert a new face embedding to database
     * @param faceEmbedding
     * @param label
     * @return id of newly inserted entry
     */
    int insertFaceVector(const cv::Mat& faceEmbedding,
                         const int label,
                         const QString& context)                                const;

    /**
     * @brief reconstructTree: reconstruct KD-Tree from data in the database
     * @return
     */
    KDTree* reconstructTree()                                                   const;

    /**
     * @brief trainData: extract train data from database
     * @return
     */
    cv::Ptr<cv::ml::TrainData> trainData()                                      const;

    /**
     * @brief insertToTreeDb : insert a new node to spatial database
     * @param nodeID
     * @param label
     * @param faceEmbedding
     * @return true if successed
     */
    bool insertToTreeDb(const int nodeID,
                        const cv::Mat& faceEmbedding)                           const;

    /**
     * @brief getClosestNeighbors : return a list of closest neighbor, limited by maxNbNeighbors and sqRange
     * @param subTree
     * @param neighborList
     * @param position
     * @param sqRange
     * @param cosThreshold
     * @param maxNbNeighbors

     * @return
     */
    QMap<double, QVector<int> > getClosestNeighborsTreeDb(const cv::Mat& position,
                                                          float sqRange,
                                                          float cosThreshold,
                                                          int maxNbNeighbors)   const;

    void clearTreeDb()                                                          const;

    /**
     * @brief clearDNNTraining : clear all trained data in the database
     * @param context
     */
    void clearDNNTraining(const QString& context = QString());
    void clearDNNTraining(const QList<int>& identities, const QString& context = QString());

private:

    void updateRangeTreeDb(int nodeId,
                           cv::Mat& minRange,
                           cv::Mat& maxRange,
                           const cv::Mat& position)                             const;
    int findParentTreeDb(const cv::Mat& nodePos,
                         bool& leftChild,
                         int& parentSplitAxis)                                  const;

    class DataNode;

    /**
     * @brief getClosestNeighborsTreeDb : return a list of closest neighbor from a sub tree, limited by maxNbNeighbors and sqRange
     * @param subTree
     * @param neighborList
     * @param position
     * @param sqRange
     * @param cosThreshold
     * @param maxNbNeighbors

     * @return
     */
    double getClosestNeighborsTreeDb(const DataNode& subTree,
                                     QMap<double, QVector<int> >& neighborList,
                                     const cv::Mat& position,
                                     float sqRange,
                                     float cosThreshold,
                                     int maxNbNeighbors)                        const;

private:

    // Disable
    FaceDb(const FaceDb&)            = delete;
    FaceDb& operator=(const FaceDb&) = delete;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_DB_H
