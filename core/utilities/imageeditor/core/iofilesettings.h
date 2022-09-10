/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-03
 * Description : IO file Settings Container.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IO_FILE_SETTINGS_H
#define DIGIKAM_IO_FILE_SETTINGS_H

// Local includes

#include "drawdecoding.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT IOFileSettings
{

public:

    IOFileSettings()
      : JPEGCompression    (75),
        JPEGSubSampling    (1),    ///< Medium sub-sampling
        PNGCompression     (9),
        TIFFCompression    (false),
        JPEG2000Compression(75),
        JPEG2000LossLess   (true),
        PGFCompression     (3),
        PGFLossLess        (true),
        HEIFCompression    (75),
        HEIFLossLess       (true),
        JXLCompression     (75),
        JXLLossLess        (true),
        WEBPCompression    (75),
        WEBPLossLess       (true),
        AVIFCompression    (75),
        AVIFLossLess       (true),
        useRAWImport       (true),
        rawImportToolIid   (QLatin1String("org.kde.digikam.plugin.rawimport.Native"))
    {
    };

    ~IOFileSettings()
    {
    };

public:

    /// JPEG quality value.
    int          JPEGCompression;

    /// JPEG chroma sub-sampling value.
    int          JPEGSubSampling;

    /// PNG compression value.
    int          PNGCompression;

    /// TIFF deflate compression.
    bool         TIFFCompression;

    /// JPEG2000 quality value.
    int          JPEG2000Compression;

    /// JPEG2000 lossless compression.
    bool         JPEG2000LossLess;

    /// PGF quality value.
    int          PGFCompression;

    /// PGF lossless compression.
    bool         PGFLossLess;

    /// HEIF quality value.
    int          HEIFCompression;

    /// HEIF lossless compression.
    bool         HEIFLossLess;

    /// JXL quality value.
    int          JXLCompression;

    /// JXL lossless compression.
    bool         JXLLossLess;

    /// WEBP quality value.
    int          WEBPCompression;

    /// WEBP lossless compression.
    bool         WEBPLossLess;

    /// AVIF quality value.
    int          AVIFCompression;

    /// AVIF lossless compression.
    bool         AVIFLossLess;

    /// Use Raw Import tool to load a RAW picture.
    bool         useRAWImport;

    QString      rawImportToolIid;

    /// RAW File decoding options.
    DRawDecoding rawDecodingSettings;
};

} // namespace Digikam

#endif // DIGIKAM_IO_FILE_SETTINGS_H
