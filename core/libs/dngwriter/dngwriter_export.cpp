/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Export to target DNG.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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

#include "dngwriter_p.h"

// Local includes

#include "dngwriterhost.h"

namespace Digikam
{

int DNGWriter::Private::exportTarget(DNGWriterHost& host,
                                     AutoPtr<dng_negative>& negative,
                                     AutoPtr<dng_image>& image)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Build DNG Negative";

    // Assign Raw image data.

    negative->SetStage1Image(image);

    // Compute linearized and range mapped image

    negative->BuildStage2Image(host);

    // Compute demosaiced image (used by preview and thumbnail)

    negative->BuildStage3Image(host);

    negative->SynchronizeMetadata();
    negative->RebuildIPTC(true);

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    // -----------------------------------------------------------------------------------------

    dng_preview_list previewList;
    dng_render negRender(host, *negative.Get());

    if (previewMode != DNGWriter::NONE)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG preview image creation";

        dng_jpeg_preview* const jpeg_preview = new dng_jpeg_preview();
        jpeg_preview->fInfo.fApplicationName.Set_ASCII(QString::fromLatin1("digiKam").toLatin1().constData());
        jpeg_preview->fInfo.fApplicationVersion.Set_ASCII(digiKamVersion().toLatin1().constData());
        jpeg_preview->fInfo.fDateTime        = orgDateTimeInfo.Encode_ISO_8601();
        jpeg_preview->fInfo.fColorSpace      = previewColorSpace_sRGB;

        negRender.SetMaximumSize(previewMode == MEDIUM ? 1280 : width);
        AutoPtr<dng_image> negImage(negRender.Render());
        dng_image_writer jpegWriter;
        jpegWriter.EncodeJPEGPreview(host, *negImage.Get(), *jpeg_preview, 5);
        AutoPtr<dng_preview> jp(static_cast<dng_preview*>(jpeg_preview));
        previewList.Append(jp);
    }

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    // -----------------------------------------------------------------------------------------

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG thumbnail creation";

    dng_image_preview* const thumbnail = new dng_image_preview();
    thumbnail->fInfo.fApplicationName.Set_ASCII(QString::fromLatin1("digiKam").toLatin1().constData());
    thumbnail->fInfo.fApplicationVersion.Set_ASCII(digiKamVersion().toLatin1().constData());
    thumbnail->fInfo.fDateTime         = orgDateTimeInfo.Encode_ISO_8601();
    thumbnail->fInfo.fColorSpace       = previewColorSpace_sRGB;

    negRender.SetMaximumSize(256);
    thumbnail->fImage.Reset(negRender.Render());
    AutoPtr<dng_preview> tn(static_cast<dng_preview*>(thumbnail));
    previewList.Append(tn);

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    // -----------------------------------------------------------------------------------------

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Creating DNG file" << outputInfo.fileName();

    dng_image_writer writer;
    dng_file_stream filestream(QFile::encodeName(dngFilePath).constData(), true);

    writer.WriteDNG(host,
                    filestream,
                    *negative.Get(),
                    &previewList,
                    dngVersion_SaveDefault,
                    !jpegLossLessCompression
                   );

    // -----------------------------------------------------------------------------------------
    // Metadata makernote cleanup using Exiv2 for some RAW file types
    // See bug #204437 and #210371, and write XMP Sidecar if necessary
    // We disable writing to RAW files, see bug #381432
/*
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (meta->load(dngFilePath))
    {
        if (inputInfo.suffix().toUpper() == QLatin1String("ORF"))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: cleanup makernotes using Exiv2";

            meta->setWriteDngFiles(true);
            meta->removeExifTag("Exif.OlympusIp.BlackLevel");
        }
        else
        {
            // Don't touch DNG file and create XMP sidecar depending of host application settings.
            meta->setWriteDngFiles(false);
        }

        meta->applyChanges();
    }
*/
    // -----------------------------------------------------------------------------------------
    // update modification time if desired

    if (updateFileDate)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Setting modification date from meta data:" << fileDate.toString();

        DFileOperations::setModificationTime(dngFilePath, fileDate);
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
