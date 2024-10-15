/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNN Model Manager defintion
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QList>

namespace Digikam
{

typedef enum _DNNLoaderType
{
    Net,
    Config,
    YuNet,
    SFace
} DNNLoaderType;

typedef enum _DNNModelUsage
{
    FaceDetection,
    FaceRecognition,
    RedeyeDetection,
    ObjectDetection,
    Aesthetic
} DNNModelUsage;

typedef QList<DNNModelUsage> DNNModelUsageList;

} // namespace Digikam
