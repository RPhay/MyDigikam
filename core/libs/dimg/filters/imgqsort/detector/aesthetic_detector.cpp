/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Aesthetic detection
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "aesthetic_detector.h"

// Qt includes

#include <QStandardPaths>
#include <QUrl>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

/*
cv::dnn::Net loadModel()
{
    cv::dnn::Net model;

    QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    QUrl    appUrl  = QUrl::fromLocalFile(appPath).adjusted(QUrl::RemoveFilename);
    appUrl.setPath(appUrl.path() + QLatin1String("share/digikam/aestheticdetector/"));

    QString nnmodel = appUrl.toLocalFile() + QLatin1String("weights_inceptionv3_299.pb");

    if (QFileInfo::exists(nnmodel))
    {
        try
        {
            qCDebug(DIGIKAM_DIMG_LOG) << "Aesthetic detector model:" << nnmodel;

#ifdef Q_OS_WIN

            model = cv::dnn::readNetFromTensorflow(nnmodel.toLocal8Bit().constData());

#else

            model = cv::dnn::readNetFromTensorflow(nnmodel.toStdString());

#endif
            model.setPreferableBackend(cv::dnn::DNN_TARGET_CPU);
            model.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_DIMG_LOG) << "cv::Exception:" << e.what();

            return model;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_DIMG_LOG) << "Default exception from OpenCV";

           return model;
        }
    }
    else
    {
        qCCritical(DIGIKAM_DIMG_LOG) << "Cannot found Aesthetic DNN model" << nnmodel;
        qCCritical(DIGIKAM_DIMG_LOG) << "Aesthetic detection feature cannot be used!";

        return model;
    }

    return model;
}
*/

cv::dnn::Net AestheticDetector::model = cv::dnn::Net();

class Q_DECL_HIDDEN AestheticDetector::Private
{

public:

    explicit Private()
    {
    }

    // cv::dnn::Net model;
};

AestheticDetector::AestheticDetector()
    : DetectorDistortion(),
      d                 (new Private)
{
}

AestheticDetector::~AestheticDetector()
{
    delete d;
}

float AestheticDetector::detect(const cv::Mat& image) const
{
    cv::Mat input = preprocess(image);

    if (!model.empty())
    {
        model.setInput(input);
        cv::Mat out = model.forward();

        return postProcess(out);
    }
    else
    {
        qCCritical(DIGIKAM_DIMG_LOG) << "Cannot load Aesthetic DNN model";

        return (-1.0F);
    }
}

cv::Mat AestheticDetector::preprocess(const cv::Mat& image) const
{
    cv::Mat img_rgb;
    cv::cvtColor(image, img_rgb, cv::COLOR_BGR2RGB);
    cv::Mat cv_resized;
    cv::resize(img_rgb, cv_resized, cv::Size(299, 299), 0, 0, cv::INTER_NEAREST_EXACT);
    cv_resized.convertTo(cv_resized, CV_32FC3);
    cv_resized   = cv_resized.mul(1.0F / 127.5F);
    subtract(cv_resized, cv::Scalar(1, 1, 1), cv_resized);

    cv::Mat blob = cv::dnn::blobFromImage(cv_resized, 1, cv::Size(299, 299), cv::Scalar(0, 0, 0), false, false);

    return blob;
}

float AestheticDetector::postProcess(const cv::Mat& modelOutput) const
{
    cv::Point maxLoc;
    cv::minMaxLoc(modelOutput, nullptr, nullptr, nullptr, &maxLoc);
    qCDebug(DIGIKAM_DIMG_LOG) << "class : " << maxLoc.x;

    return float(maxLoc.x);
}

bool AestheticDetector::loadModel()
{
    if (!model.empty())
    {
        return true;
    }

    QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    QUrl    appUrl  = QUrl::fromLocalFile(appPath).adjusted(QUrl::RemoveFilename);
    appUrl.setPath(appUrl.path() + QLatin1String("digikam/aestheticdetector/"));

    QString nnmodel = appUrl.toLocalFile() + QLatin1String("weights_inceptionv3_299.pb");

    if (QFileInfo::exists(nnmodel))
    {
        try
        {
            qCDebug(DIGIKAM_DIMG_LOG) << "Aesthetic detector model:" << nnmodel;

#ifdef Q_OS_WIN

            model = cv::dnn::readNetFromTensorflow(nnmodel.toLocal8Bit().constData());

#else

            model = cv::dnn::readNetFromTensorflow(nnmodel.toStdString());

#endif
            model.setPreferableBackend(cv::dnn::DNN_TARGET_CPU);
            model.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_DIMG_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_DIMG_LOG) << "Default exception from OpenCV";

           return false;
        }
    }
    else
    {
        qCCritical(DIGIKAM_DIMG_LOG) << "Cannot found Aesthetic DNN model" << nnmodel;
        qCCritical(DIGIKAM_DIMG_LOG) << "Aesthetic detection feature cannot be used!";

        return false;
    }

    return true;
}

void AestheticDetector::unloadModel()
{
    if (!model.empty())
    {
        model = cv::dnn::Net();
    }
}

} // namespace Digikam
