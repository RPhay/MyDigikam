/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Preprocessor for openface nn model
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "openfacepreprocessor.h"

// Qt includes

#include <QFile>
#include <QTime>
#include <QString>
#include <QDataStream>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "fullobjectdetection.h"

namespace Digikam
{

// --------------------------------------- Static global variables -----------------------------------

/** Template for face landmark to perform alignment with open face
  * This variable must be declared as static so that it is allocated as long as
  * digiKam is still running. We need that because this variable is the internal data
  * for matrix faceTemplate below.
  */
static float FACE_TEMPLATE[3][2] = {
                                       { 18.639072F, 16.249624F },
                                       { 75.73048F,  15.18443F  },
                                       { 47.515285F, 49.38637F  }
                                   };

// ---------------------------------------------------------------------------------------------------

OpenfacePreprocessor::OpenfacePreprocessor()
    : outImageSize          (cv::Size(96, 96)),
      faceTemplate          (cv::Mat(3, 2, CV_32F, &FACE_TEMPLATE)),
      outerEyesNosePositions( { 36, 45, 33 } )
{
}

bool OpenfacePreprocessor::loadModels()
{
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine"),
                                             QStandardPaths::LocateDirectory);

    QString data    = QLatin1String("shapepredictor.dat");
    QString spdata  = appPath + QLatin1Char('/') + data;

    QFile model(spdata);
    RedEye::ShapePredictor* const temp = new RedEye::ShapePredictor();

    qCDebug(DIGIKAM_FACEDB_LOG) << "Start reading shape predictor file";

    if (model.open(QIODevice::ReadOnly))
    {
        QDataStream dataStream(&model);
        dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        dataStream >> *temp;
        sp = *temp;
        model.close();
    }
    else
    {
        delete temp;

        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found faces engine model" << data;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }

    delete temp;

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish reading shape predictor file";

    return true;
}

cv::Mat OpenfacePreprocessor::process(const cv::Mat& image)
{
    if (!sp.num_parts())
    {
        return image;
    }

    int type = image.type();
    qCDebug(DIGIKAM_FACEDB_LOG) << "type: " << type;

    cv::Mat gray;

    if ((type == CV_8UC3) || (type == CV_16UC3))
    {
        cv::cvtColor(image, gray, CV_RGB2GRAY);   // 3 channels
    }
    else
    {
        cv::cvtColor(image, gray, CV_RGBA2GRAY);  // 4 channels
    }

    if ((type == CV_16UC3) || (type == CV_16UC4))
    {
        gray.convertTo(gray, CV_8UC1, 1 / 255.0);
    }

    cv::Rect new_rect(0, 0, image.cols, image.rows);
    cv::Mat landmarks(3, 2, CV_32F);

    mutex.lock();
    FullObjectDetection object = sp(gray, new_rect);
    mutex.unlock();

    for (size_t i = 0 ; i < outerEyesNosePositions.size() ; ++i)
    {
        int index                      = outerEyesNosePositions[i];
        landmarks.at<float>((int)i, 0) = object.part(index)[0];
        landmarks.at<float>((int)i, 1) = object.part(index)[1];
/*
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "index = " << index
                                         << ", landmarks: (" << landmarks.at<float>(i, 0)
                                         << ", " << landmarks.at<float>(i, 1) << ")" << Qt::endl;
*/
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "Full object detection and landmard computation finished";

    // qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing landmark in " << timer.restart() << " ms";

    cv::Mat affineTransformMatrix = cv::getAffineTransform(landmarks, faceTemplate);
    cv::Mat alignedFace;
    cv::warpAffine(image, alignedFace, affineTransformMatrix, outImageSize);

    if (alignedFace.empty())
    {
        qCDebug(DIGIKAM_FACEDB_LOG) << "Face alignment failed!";
        return image;
    }
    else
    {
        qCDebug(DIGIKAM_FACEDB_LOG) << "Align face finished";
    }

    return alignedFace;
}

} // namespace Digikam
