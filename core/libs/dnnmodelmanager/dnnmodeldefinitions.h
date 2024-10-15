/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-31
 * Description : digiKam global static QNetworkAccessManager
 *
 * SPDX-FileCopyrightText: 2022 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes
#include <QList>

// Local includes

namespace Digikam
{

typedef enum enum_DNNLoaderType 
{
    Net,
    Config,
    YuNet,
    SFace
} DNNLoaderType;

typedef enum enum_DNNModelUsage
{
    FaceDetection,
    FaceRecognition,
    RedeyeDetection,
    ObjectDetection,
    Aesthetic
} DNNModelUsage;

typedef QList<DNNModelUsage> DNNModelUsageList;

} // namespace Digikam
