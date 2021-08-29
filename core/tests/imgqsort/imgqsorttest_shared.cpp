/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to detect image quality level - shared code
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imgqsorttest_shared.h"

// Qt includes

#include <QStringList>
#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QTest>

// Local includes

#include "dimg.h"
#include "previewloadthread.h"
#include "imagequalityparser.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "imagequalitycontainer.h"
#include "dpluginloader.h"


namespace Digikam
{

ImageQualityContainer ImgQSortTest_ArrangeSettings (DetectionType type)
{
    qCDebug(DIGIKAM_TESTS_LOG) << "Process images for detection type "<<type;

    qCInfo(DIGIKAM_TESTS_LOG)  << "Detection type (0:Blur, 1:Noise, 2:Compression, 3:Exposure, 4: General)";

    ImageQualityContainer settings;

    settings.detectBlur         = false;
    settings.detectNoise        = false;
    settings.detectCompression  = false;
    settings.detectExposure     = false;

    switch (type)
    {
        case DETECTNOISE:
            settings.detectNoise        = true;
            break;

        case DETECTCOMPRESSION:
            settings.detectCompression  = true;
            break;

        case DETECTEXPOSURE:
            settings.detectExposure     = true;
            break;

        case DETECTBLUR:
            settings.detectBlur         = true;
            break;

        default:
            settings.detectBlur         = true;
            settings.detectCompression  = true;
            settings.detectNoise        = true;
            settings.detectExposure     = true;
            break;
    }

    return settings;
}

ImageQualityContainer ImgQSortTest_ArrangeCustomSettings(const CustomDetection& customSetting)
{
    qCInfo(DIGIKAM_TESTS_LOG)  << "Detection type activate Blur "<< customSetting.detectBlur
                               << "Noise "      << customSetting.detectNoise
                               << "Compression "<< customSetting.detectCompression
                               << "Exposure "   << customSetting.detectExposure;

    ImageQualityContainer settings;

    settings.detectBlur         = customSetting.detectBlur;
    settings.detectCompression  = customSetting.detectCompression;
    settings.detectNoise        = customSetting.detectNoise;
    settings.detectExposure     = customSetting.detectExposure;

    return settings;
}

QHash<QString, int> ImgQSortTest_ParseTestImagesCore(const ImageQualityContainer& settings, const QFileInfoList& list)
{
    qCDebug(DIGIKAM_TESTS_LOG) << "Quality Detection Settings:" << settings;

    QHash<QString, int> results;

    foreach (const QFileInfo& inf, list)
    {
        QString path = inf.filePath();
        qCDebug(DIGIKAM_TESTS_LOG) << path;

        DImg dimg    = PreviewLoadThread::loadFastSynchronously(path, 1024);

        if (dimg.isNull())
        {
            qCDebug(DIGIKAM_TESTS_LOG) << path << "File cannot be loaded...";
        }

        PickLabel pick;
        ImageQualityParser parser(dimg, settings, &pick);
        parser.startAnalyse();
        results.insert( path.split(QLatin1String("/")).last(), pick);
    }

    qCInfo(DIGIKAM_TESTS_LOG) << "Quality Results (0:None, 1:Rejected, 2:Pending, 3:Accepted):";

    for (const auto& image_name: results.keys())
    {
        qCInfo(DIGIKAM_TESTS_LOG) << "==>" << image_name << ":" << results.value(image_name);
    }

    return results;
}

QHash<QString, int> ImgQSortTest_ParseTestImagesDefautDetection(DetectionType type, const QFileInfoList& list)
{
    ImageQualityContainer settings = ImgQSortTest_ArrangeSettings(type);

    return ImgQSortTest_ParseTestImagesCore(settings, list);
}

QHash<QString, int> ImgQSortTest_ParseTestImagesCustomDetection(const CustomDetection& customSetting, const QFileInfoList& list)
{
    ImageQualityContainer settings = ImgQSortTest_ArrangeCustomSettings(customSetting);

    return ImgQSortTest_ParseTestImagesCore(settings, list);
}

} // namespace Digikam
