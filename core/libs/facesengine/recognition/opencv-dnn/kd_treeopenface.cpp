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

#include "kd_treeopenface.h"

// Qt includes

#include <QMutex>
#include <QString>
#include <QFileInfo>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "kd_nodeopenface.h"

namespace Digikam
{

KDTreeOpenFace::KDTreeOpenFace(
    int dim,
    int threshold
)
    : KDTreeBase(dim, threshold)
{
}

KDTreeOpenFace::~KDTreeOpenFace()
{
}

KDNodeBase* KDTreeOpenFace::createNode(const cv::Mat& nodePos,
                                       const int identity,
                                       int splitAxis,
                                       int dimension)
{
    return new KDNodeOpenFace(
               nodePos,
               identity,
               splitAxis,
               dimension
           );
}

} // namespace Digikam
