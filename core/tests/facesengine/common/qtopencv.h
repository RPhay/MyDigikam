/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-06-16
 * Description : Functions to convert between OpenCV's cv::Mat and Qt's QImage and QPixmap.
 *               Partially inspired from:
 *               https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
 *
 * Copyright (C)      2013 by Andy Maloney <asmaloney at gmail dot com>
 * Copyright (C) 2015-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_QT_OPENCV_H
#define DIGIKAM_QT_OPENCV_H

// Qt includes

#include <QImage>
#include <QPixmap>

// Local includes

#include "digikam_opencv.h"
#include "digikam_debug.h"

namespace QtOpenCV
{
    // NOTE: This does not cover all cases - it should be easy to add new ones as required.

    inline QImage cvMatToQImage(const cv::Mat& inMat)
    {
        switch (inMat.type())
        {
            // 8-bit, 4 channel

            case CV_8UC4:
            {
                QImage image(inMat.data,
                             inMat.cols,
                             inMat.rows,
                             static_cast<int>(inMat.step),
                             QImage::Format_ARGB32);

                return image;
            }

            // 8-bit, 3 channel

            case CV_8UC3:
            {
                QImage image(inMat.data,
                             inMat.cols,
                             inMat.rows,
                             static_cast<int>(inMat.step),
                             QImage::Format_RGB888);

                return image.rgbSwapped();
            }

            // 8-bit, 1 channel

            case CV_8UC1:
            {
                QImage image(inMat.data,
                          inMat.cols,
                          inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_Grayscale8);

                return image;
            }

            default:
            {
                qCWarning(DIGIKAM_TESTS_LOG) << "cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
                break;
            }
        }

        return QImage();
    }

    // ----------------------------------------------------------------------------------

    inline QPixmap cvMatToQPixmap(const cv::Mat& inMat)
    {
        return QPixmap::fromImage(cvMatToQImage(inMat));
    }

    // ----------------------------------------------------------------------------------

    /**
     * If inImage exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inImage's
     * data with the cv::Mat directly
     * NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
     * NOTE: This does not cover all cases - it should be easy to add new ones as required.
     */
    inline cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true)
    {
        switch (inImage.format())
        {
            // 8-bit, 4 channel

            case QImage::Format_ARGB32:
            case QImage::Format_ARGB32_Premultiplied:
            {
                cv::Mat mat(inImage.height(),
                            inImage.width(),
                            CV_8UC4,
                            const_cast<uchar*>(inImage.bits()),
                            static_cast<size_t>(inImage.bytesPerLine()));

                return (inCloneImageData ? mat.clone() : mat);
            }

            // 8-bit, 3 channel

            case QImage::Format_RGB32:
            case QImage::Format_RGB888:
            {
                if (!inCloneImageData)
                {
                    qCWarning(DIGIKAM_TESTS_LOG) << "QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
                }

                QImage swapped;

                if (inImage.format() == QImage::Format_RGB32)
                {
                    swapped = inImage.convertToFormat(QImage::Format_RGB888);
                }
                else
                {
                    swapped = inImage.rgbSwapped();
                }

                return cv::Mat(swapped.height(),
                               swapped.width(),
                               CV_8UC3,
                               const_cast<uchar*>(swapped.bits()),
                               static_cast<size_t>(swapped.bytesPerLine())
                              ).clone();
            }

            // 8-bit, 1 channel

            case QImage::Format_Indexed8:
            {
                cv::Mat mat(inImage.height(),
                            inImage.width(),
                            CV_8UC1,
                            const_cast<uchar*>(inImage.bits()),
                            static_cast<size_t>(inImage.bytesPerLine()));

                return (inCloneImageData ? mat.clone() : mat);
            }

            default:
            {
                qCWarning(DIGIKAM_TESTS_LOG) << "QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
                break;
            }
        }

        return cv::Mat();
    }


    // ----------------------------------------------------------------------------------

    /**
     * If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
     * with the cv::Mat directly
     * NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
     */
    inline cv::Mat QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData = true)
    {
        return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
    }

} // namespace QtOpenCV

#endif // DIGIKAM_QT_OPENCV_H
