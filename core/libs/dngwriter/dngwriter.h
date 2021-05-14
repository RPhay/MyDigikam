/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Jens Mueller <tschenser at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DNG_WRITER_H
#define DIGIKAM_DNG_WRITER_H

// Qt includes

#include <QString>

// Local includes

#include "drawdecoder.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNGWriter
{

public:

    enum JPEGPreview
    {
        NONE = 0,                           ///< No preview will be generated.
        MEDIUM,                             ///< A medium size preview will be generated.
        FULL_SIZE                           ///< A full size preview  will be generated.
    };

    enum ConvertError
    {
        PROCESS_CONTINUE       =  1,        ///< Current stages is done.
        PROCESS_COMPLETE       =  0,        ///< All stages done.
        PROCESS_FAILED         = -1,        ///< A failure happen while processing.
        PROCESS_CANCELED       = -2,        ///< User has canceled processing.
        FILE_NOT_SUPPORTED     = -3,        ///< Raw file format is not supported by converter.
        DNG_SDK_INTERNAL_ERROR = -4         ///< Adobe DNG SDK has generated an error while processing.
    };

public:

    explicit DNGWriter();
    ~DNGWriter();

    void setInputFile(const QString& filePath);
    void setOutputFile(const QString& filePath);

    QString inputFile()             const;
    QString outputFile()            const;

    void setCompressLossLess(bool b);
    bool compressLossLess()         const;

    void setUpdateFileDate(bool b);
    bool updateFileDate()           const;

    void setBackupOriginalRawFile(bool b);
    bool backupOriginalRawFile()    const;

    void setPreviewMode(int mode);
    int  previewMode()              const;

    int  convert();
    void cancel();
    void reset();

public:

    static QString xmpSdkVersion();
    static QString dngSdkVersion();

public:

    // Declared public for DNGWriterHost class.
    class Private;

private:

    // Disable
    DNGWriter(const DNGWriter&)            = delete;
    DNGWriter& operator=(const DNGWriter&) = delete;

private:

    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DNG_WRITER_H
