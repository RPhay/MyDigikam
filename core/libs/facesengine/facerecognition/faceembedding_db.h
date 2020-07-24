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

#ifndef FACEEMBEDDING_DB_H
#define FACEEMBEDDING_DB_H

#include "digikam_opencv.h"
#include "kd_tree.h"

namespace RecognitionTest
{
class FaceEmbeddingDb
{
public:
    explicit FaceEmbeddingDb();
    ~FaceEmbeddingDb();

public:
    /**
     * @brief insert : insert a new face embedding to database
     * @param faceEmbedding
     * @param label
     * @return id of newly inserted entry
     */
    int insert(const cv::Mat& faceEmbedding, const int label, const QString& context) const;

    /**
     * @brief reconstructTree: reconstruct KD-Tree from data in the database
     * @return
     */
    KDTree* reconstructTree() const;

    /**
     * @brief trainData: extract train data from database
     * @return
     */
    cv::Ptr<cv::ml::TrainData> trainData() const;

private:

    class Private;
    Private* d;
};
}
#endif // FACEEMBEDDING_DB_H
