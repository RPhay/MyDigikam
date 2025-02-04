/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Integrated, multithread object detection / recognition
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsclassifierminmax.h"

// Qt includes

#include <QList>

// local includes

#include "digikam_debug.h"

namespace Digikam
{

AutotagsClassifierMinmax::AutotagsClassifierMinmax(float _threshold, const QString& predefinedClassFile)
    : AutotagsClassifierBase(_threshold, predefinedClassFile)
{
}

int AutotagsClassifierMinmax::predict(const cv::Mat& target) const
{
    Q_ASSERT(target.rows == 1);

    // Get the prediction

    cv::Point classIdPoint;
    double confidence  = 0.0;
    cv::minMaxLoc(target.reshape(1, 1), 0, &confidence, 0, &classIdPoint);
    int classId       = classIdPoint.x;

    qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsClassifierMinmax::predict: Class ID:"
                                        << classId << "Confidence:" << confidence
                                        << "Predefined class:" << predefinedClasses[classId];

    if (confidence > threshold)
    {
        return classId;
    }

    return -1;
}

int AutotagsClassifierMinmax::predict(const cv::UMat& target) const
{
    return predict(target.getMat(cv::ACCESS_READ));
}

QList<int> AutotagsClassifierMinmax::predictMulti(const QList<cv::Mat>& targets) const
{
    QList<int> result;

    int label = predict(targets[0]);

    if (-1 != label)
    {
        result << label;
    }

    return result;
}

QList<int> AutotagsClassifierMinmax::predictMulti(const QList<cv::UMat>& targets) const
{ 
    QList<int> result;

    int label = predict(targets[0]);

    if (-1 != label)
    {
        result << label;
    }

    return result;
}

} // namespace Digikam
