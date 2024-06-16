/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a tool to create panorama by fusion of several images.
 *
 * SPDX-FileCopyrightText: 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>
#include <QImage>
#include <QMetaType>
#include <QMap>
#include <QUrl>

namespace DigikamGenericPanoramaPlugin
{

enum PanoAction
{
    PANO_NONE = 0,                   // 0
    PANO_PREPROCESS_INPUT,           // 1
    PANO_CREATEPTO,                  // 2
    PANO_CPFIND,                     // 3
    PANO_CPCLEAN,                    // 4
    PANO_OPTIMIZE,                   // 5
    PANO_AUTOCROP,                   // 6
    PANO_CREATEPREVIEWPTO,           // 7
    PANO_CREATEMK,                   // 8
    PANO_CREATEMKPREVIEW,            // 9
    PANO_CREATEFINALPTO,             // 10
    PANO_NONAFILE,                   // 11
    PANO_NONAFILEPREVIEW,            // 12
    PANO_STITCH,                     // 13
    PANO_STITCHPREVIEW,              // 14
    PANO_HUGINEXECUTOR,              // 15
    PANO_HUGINEXECUTORPREVIEW,       // 16
    PANO_COPY                        // 17
};

typedef enum
{
    JPEG,
    TIFF,
    HDR
}
PanoramaFileType;

class PanoramaPreprocessedUrls
{
public:

    PanoramaPreprocessedUrls()          = default;
    explicit PanoramaPreprocessedUrls(const QUrl& preprocessed, const QUrl& preview)
        : preprocessedUrl(preprocessed),
          previewUrl     (preview)
    {
    }

    ~PanoramaPreprocessedUrls()         = default;

public:

    QUrl preprocessedUrl;              ///< Can be an original file or a converted version, depending on the original file type
    QUrl previewUrl;                   ///< The JPEG preview version, accordingly of preprocessedUrl constant.
};

typedef QMap<QUrl, PanoramaPreprocessedUrls> PanoramaItemUrlsMap;   ///< Map between original Url and processed temp Urls.

// ----------------------------------------------------------------------------------------------------------

struct PanoActionData
{
    PanoActionData() = default;

    bool                starting    = false;
    bool                success     = false;

    QString             message;    ///< Usually, an error message

    int                 id          = 0;

    PanoAction          action      = PANO_NONE;
};

} // namespace DigikamGenericPanoramaPlugin

Q_DECLARE_METATYPE(DigikamGenericPanoramaPlugin::PanoActionData)
Q_DECLARE_METATYPE(DigikamGenericPanoramaPlugin::PanoramaPreprocessedUrls)
