/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-09-02
 * Description : A convenience class for a standalone face detector
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "facedetector.h"

// Qt includes

#include <QSharedData>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"

#ifdef USE_DNN_DETECTION_BACKEND
#   include "opencvdnnfacedetector.h"
#else
#   include "opencvfacedetector.h"
#endif

namespace Digikam
{

class Q_DECL_HIDDEN FaceDetector::Private : public QSharedData
{
public:

    explicit Private()
        :

#ifdef USE_DNN_DETECTION_BACKEND

          m_dnnDetectorBackend(nullptr)

#else

          m_haarDetectorbackend(nullptr)

#endif

    {
    }

    ~Private()
    {

#ifdef USE_DNN_DETECTION_BACKEND

        delete m_dnnDetectorBackend;

#else

        delete m_haarDetectorbackend;

#endif

    }

#ifdef USE_DNN_DETECTION_BACKEND

    OpenCVDNNFaceDetector* backend()
    {
        if (!m_dnnDetectorBackend)
        {
            m_dnnDetectorBackend = new OpenCVDNNFaceDetector(DetectorNNModel::SSDMOBILENET);
        }

        return m_dnnDetectorBackend;
    }

    const OpenCVDNNFaceDetector* constBackend() const
    {
        return m_dnnDetectorBackend;
    }

#else

    OpenCVFaceDetector* backend()
    {
        if (!m_haarDetectorbackend)
        {
            QStringList cascadeDirs;
            cascadeDirs << QString::fromUtf8(OPENCV_ROOT_PATH) + QLatin1String("/haarcascades");

            qCDebug(DIGIKAM_FACESENGINE_LOG) << "Try to find OpenCV Haar Cascade files in this directory: " << cascadeDirs;

            m_haarDetectorbackend = new OpenCVFaceDetector(cascadeDirs);
            applyParameters();
        }

        return m_haarDetectorbackend;
    }

    const OpenCVFaceDetector* constBackend() const
    {
        return m_haarDetectorbackend;
    }

#endif

    void applyParameters()
    {
        if (!backend())
        {
            return;
        }

#ifdef USE_DNN_DETECTION_BACKEND

        // TODO

#else

        for (QVariantMap::const_iterator it = m_parameters.constBegin() ;
             it != m_parameters.constEnd() ; ++it)
        {
            if      (it.key() == QLatin1String("accuracy"))
            {
                backend()->setAccuracy(it.value().toDouble());
            }
            else if (it.key() == QLatin1String("speed"))
            {
                backend()->setAccuracy(1.0 - it.value().toDouble());
            }
            else if (it.key() == QLatin1String("specificity"))
            {
                backend()->setSpecificity(it.value().toDouble());
            }
            else if (it.key() == QLatin1String("sensitivity"))
            {
                backend()->setSpecificity(1.0 - it.value().toDouble());
            }
        }

#endif

    }

public:

    QVariantMap            m_parameters;

private:

#ifdef USE_DNN_DETECTION_BACKEND

    OpenCVDNNFaceDetector* m_dnnDetectorBackend;

#else

    OpenCVFaceDetector*    m_haarDetectorbackend;

#endif

};

// ---------------------------------------------------------------------------------

FaceDetector::FaceDetector()
    : d(new Private)
{
}

FaceDetector::FaceDetector(const FaceDetector& other)
    : d(other.d)
{
}

FaceDetector& FaceDetector::operator=(const FaceDetector& other)
{
    d = other.d;
    return *this;
}

FaceDetector::~FaceDetector()
{
    // TODO: implement reference counter
}

QString FaceDetector::backendIdentifier() const
{

#ifdef USE_DNN_DETECTION_BACKEND

    return QLatin1String("Deep Neural Network");

#else

    return QLatin1String("Haar Cascades");

#endif

}

QList<QRectF> FaceDetector::detectFaces(const QImage& image, const QSize& originalSize)
{
    QList<QRectF> result;

    if (image.isNull() || !image.size().isValid())
    {
        return result;
    }

    try
    {

#ifdef USE_DNN_DETECTION_BACKEND

        Q_UNUSED(originalSize);

        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = d->backend()->prepareForDetection(image, paddedSize);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, paddedSize);
        result                = toRelativeRects(absRects,
                                                QSize(cvImage.cols - 2*paddedSize.width,
                                                      cvImage.rows - 2*paddedSize.height));

#else

        cv::Size cvOriginalSize;

        if (originalSize.isValid())
        {
            cvOriginalSize = cv::Size(originalSize.width(), originalSize.height());
        }
        else
        {
            cvOriginalSize = cv::Size(image.width(), image.height());
        }

        cv::Mat cvImage       = d->backend()->prepareForDetection(image);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, cvOriginalSize);
        result                = toRelativeRects(absRects, QSize(cvImage.cols, cvImage.rows));

#endif

        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QRectF> FaceDetector::detectFaces(const DImg& image, const QSize& originalSize)
{
    QList<QRectF> result;

    if (image.isNull() || !image.size().isValid())
    {
        return result;
    }

    try
    {

#ifdef USE_DNN_DETECTION_BACKEND

        Q_UNUSED(originalSize);

        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = d->backend()->prepareForDetection(image, paddedSize);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, paddedSize);
        result                = toRelativeRects(absRects,
                                                QSize(cvImage.cols - 2*paddedSize.width,
                                                      cvImage.rows - 2*paddedSize.height));

#else

        cv::Size cvOriginalSize;

        if (originalSize.isValid())
        {
            cvOriginalSize = cv::Size(originalSize.width(), originalSize.height());
        }
        else
        {
            cvOriginalSize = cv::Size(image.width(), image.height());
        }

        cv::Mat cvImage       = d->backend()->prepareForDetection(image);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, cvOriginalSize);
        result                = toRelativeRects(absRects, QSize(cvImage.cols, cvImage.rows));

#endif

        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QRectF> FaceDetector::detectFaces(const QString& imagePath)
{

#ifdef USE_DNN_DETECTION_BACKEND

    QList<QRectF> result;

    try
    {
        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = d->backend()->prepareForDetection(imagePath, paddedSize);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, paddedSize);
        result                = toRelativeRects(absRects,
                                                QSize(cvImage.cols - 2*paddedSize.width,
                                                      cvImage.rows - 2*paddedSize.height));
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch(...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;

#else

    DImg img;

    if (!img.load(imagePath))
    {
        return QList<QRectF>();
    }

    return detectFaces(img, img.size());

#endif

}

void FaceDetector::setParameter(const QString& parameter, const QVariant& value)
{
    d->m_parameters.insert(parameter, value);
    d->applyParameters();
}

void FaceDetector::setParameters(const QVariantMap& parameters)
{
    for (QVariantMap::const_iterator it = parameters.begin() ; it != parameters.end() ; ++it)
    {
        d->m_parameters.insert(it.key(), it.value());
    }

    d->applyParameters();
}

QVariantMap FaceDetector::parameters() const
{
    return d->m_parameters;
}

int FaceDetector::recommendedImageSize(const QSize& availableSize) const
{
    Q_UNUSED(availableSize);

#ifdef USE_DNN_DETECTION_BACKEND

    return OpenCVDNNFaceDetector::recommendedImageSizeForDetection();

#else

    return OpenCVFaceDetector::recommendedImageSizeForDetection();

#endif

}

// -- Static methods -------------------------------------------------------------

QRectF FaceDetector::toRelativeRect(const QRect& abs, const QSize& s)
{
    if (s.isEmpty())
    {
        return QRectF();
    }

    return QRectF(qreal(abs.x())      / qreal(s.width()),
                  qreal(abs.y())      / qreal(s.height()),
                  qreal(abs.width())  / qreal(s.width()),
                  qreal(abs.height()) / qreal(s.height()));
}

QRect FaceDetector::toAbsoluteRect(const QRectF& rel, const QSize& s)
{
    return QRectF(rel.x()      * s.width(),
                  rel.y()      * s.height(),
                  rel.width()  * s.width(),
                  rel.height() * s.height()).toRect();
}

QList<QRectF> FaceDetector::toRelativeRects(const QList<QRect>& absoluteRects, const QSize& size)
{
    QList<QRectF> result;

    foreach (const QRect& r, absoluteRects)
    {
        result << toRelativeRect(r, size);
    }

    return result;
}

QList<QRect> FaceDetector::toAbsoluteRects(const QList<QRectF>& relativeRects, const QSize& size)
{
    QList<QRect> result;

    foreach (const QRectF& r, relativeRects)
    {
        result << toAbsoluteRect(r, size);
    }

    return result;
}

} // namespace Digikam
