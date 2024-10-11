/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Node of KD-Tree for vector space partitioning
 *
 * SPDX-FileCopyrightText: 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <vector>

// Qt includes

#include <QVector>
#include <QMap>

// Local include

#include "digikam_opencv.h"
#include "kd_nodebase.h"

namespace Digikam
{

class KDNodeSFace: public KDNodeBase
{
public:

    explicit KDNodeSFace(const cv::Mat& nodePos,
                         const int      identity,
                         int            splitAxis,
                         int            dimension);
                         // cv::Ptr<cv::FaceRecognizerSF> net);
    virtual ~KDNodeSFace();

protected:

    KDNodeBase* createNode(const cv::Mat& nodePos,
                           const int identity,
                           int splitAxis,
                           int dimension) override;

    KDNodeBase::NodeCompareResult nodeCompare(
                                              const cv::Mat& queryPosition,
                                              const cv::Mat& currentPosition,
                                              float sqRange,
                                              float cosThreshold,
                                              int nbDimension
                                             ) const override;

private:

    // // hide this contstructor so we don't use it
    // explicit KDNodeSFace(const cv::Mat& nodePos,
    //                 const int      identity,
    //                 int            splitAxis,
    //                 int            dimension);

    // Disable
    KDNodeSFace(const KDNodeSFace&)             = delete;
    KDNodeSFace& operator=(const KDNodeSFace&)  = delete;

private:

    // cv::Ptr<cv::FaceRecognizerSF> m_net         = nullptr;
};

} // namespace Digikam
