/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-09-02
 * Description : A convenience class for a standalone face detector
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "opencvdnnfacedetector.h"

namespace Digikam
{

class Q_DECL_HIDDEN FaceDetector::Private : public QSharedData
{
public:

    explicit Private()
        : m_dnnDetectorBackend(nullptr)
    {
    }

    ~Private()
    {
        delete m_dnnDetectorBackend;
    }

    OpenCVDNNFaceDetector* backend()
    {
        if (!m_dnnDetectorBackend)
        {
            if (m_parameters.contains(QLatin1String("useyolov3")) &&
                m_parameters.value(QLatin1String("useyolov3")).toBool())
            {
                m_dnnDetectorBackend = new OpenCVDNNFaceDetector(DetectorNNModel::YOLO);
            }
            else
            {
                m_dnnDetectorBackend = new OpenCVDNNFaceDetector(DetectorNNModel::SSDMOBILENET);
            }
        }

        return m_dnnDetectorBackend;
    }

    const OpenCVDNNFaceDetector* constBackend() const
    {
        return m_dnnDetectorBackend;
    }

    void applyParameters()
    {
        if (!backend())
        {
            return;
        }

        // TODO Handle settings

/*
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
*/
    }

public:

    QVariantMap            m_parameters;

private:

    OpenCVDNNFaceDetector* m_dnnDetectorBackend;
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
    return QLatin1String("Deep Neural Network");
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
        Q_UNUSED(originalSize);

        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = d->backend()->prepareForDetection(image, paddedSize);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, paddedSize);
        result                = toRelativeRects(absRects,
                                                QSize(cvImage.cols - 2*paddedSize.width,
                                                      cvImage.rows - 2*paddedSize.height));

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
        Q_UNUSED(originalSize);

        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = d->backend()->prepareForDetection(image, paddedSize);
        QList<QRect> absRects = d->backend()->detectFaces(cvImage, paddedSize);
        result                = toRelativeRects(absRects,
                                                QSize(cvImage.cols - 2*paddedSize.width,
                                                      cvImage.rows - 2*paddedSize.height));
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
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
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

    return OpenCVDNNFaceDetector::recommendedImageSizeForDetection();
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
