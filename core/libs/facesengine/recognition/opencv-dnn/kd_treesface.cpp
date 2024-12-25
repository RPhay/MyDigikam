/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Implementation of KD-Tree for vector space partitioning.
 *
 * SPDX-FileCopyrightText: 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "kd_treesface.h"

// Qt includes

#include <QMutex>
#include <QString>
#include <QFileInfo>
#include <QStandardPaths>

// local includes

#include "digikam_debug.h"
#include "kd_nodesface.h"

namespace Digikam
{

KDTreeSFace::KDTreeSFace(
    int dim,
    int threshold
)
    : KDTreeBase(dim, threshold)
{
}

KDTreeSFace::~KDTreeSFace()
{
}

KDNodeBase* KDTreeSFace::createNode(const cv::Mat& nodePos,
                                    const int identity,
                                    int splitAxis,
                                    int dimension)
{
    return new KDNodeSFace(
               nodePos,
               identity,
               splitAxis,
               dimension
           );
}

} // namespace Digikam
