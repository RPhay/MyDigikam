/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
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

int DNGWriter::convert()
{
    int ret;
    d->cancel = false;

    try
    {
        if (inputFile().isEmpty())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: No input file to convert. Aborted...";
            return PROCESS_FAILED;
        }

        d->inputInfo          = QFileInfo(inputFile());
        QString   dngFilePath = outputFile();

        if (dngFilePath.isEmpty())
        {
            dngFilePath = QString(d->inputInfo.completeBaseName() + QLatin1String(".dng"));
        }

        QFileInfo  outputInfo(dngFilePath);
        QByteArray rawData;
        QScopedPointer<DRawInfo> identify(new DRawInfo);
        QScopedPointer<DRawInfo> identifyMake(new DRawInfo);

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data from " << d->inputInfo.fileName();

        QPointer<DRawDecoder> rawProcessor(new DRawDecoder);

        if (!rawProcessor->rawFileIdentify(*identifyMake.data(), inputFile()))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Reading RAW file failed. Aborted...";

            return PROCESS_FAILED;
        }

        dng_rect activeArea;

        // TODO: need to get correct default crop size to avoid artifacts at the borders

        int activeWidth  = 0;
        int activeHeight = 0;
        int outputHeight = 0;
        int outputWidth  = 0;

        if ((identifyMake->orientation == 5) || (identifyMake->orientation == 6))
        {
            outputHeight = identifyMake->outputSize.width();
            outputWidth  = identifyMake->outputSize.height();
        }
        else
        {
            outputHeight = identifyMake->outputSize.height();
            outputWidth  = identifyMake->outputSize.width();
        }

        if (!rawProcessor->extractRAWData(inputFile(), rawData, *identify, 0))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data failed. Aborted..." ;

            return FILE_NOT_SUPPORTED;
        }

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Raw data loaded:" ;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Data Size:     " << rawData.size() << " bytes";
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Date:          " << identify->dateTime.toString(Qt::ISODate);
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Make:          " << identify->make;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Model:         " << identify->model;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- ImageSize:     " << identify->imageSize.width()  << "x" << identify->imageSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- FullSize:      " << identify->fullSize.width()   << "x" << identify->fullSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- OutputSize:    " << identify->outputSize.width() << "x" << identify->outputSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Orientation:   " << identify->orientation;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Top margin:    " << identify->topMargin;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Left margin:   " << identify->leftMargin;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Filter:        " << identify->filterPattern;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Colors:        " << identify->rawColors;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Black:         " << identify->blackPoint;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- White:         " << identify->whitePoint;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- CAM->XYZ:" ;

        QString matrixVal;

        for (int i = 0 ; i < 4 ; ++i)
        {
            qCDebug(DIGIKAM_GENERAL_LOG)
                     << "                   "
                     << QString().asprintf("%03.4f  %03.4f  %03.4f", identify->cameraXYZMatrix[i][0],
                                                                     identify->cameraXYZMatrix[i][1],
                                                                     identify->cameraXYZMatrix[i][2]);
        }

        // Check if CFA layout is supported by DNG SDK.

        Private::DNGBayerPattern bayerPattern = Private::Unknown;
        uint32 filter                         = 0;
        bool fujiRotate90                     = false;

        // Standard bayer layouts

        if      (identify->filterPattern == QLatin1String("GRBGGRBGGRBGGRBG"))
        {
            bayerPattern = Private::Standard;
            filter       = 0;
        }
        else if (identify->filterPattern == QLatin1String("RGGBRGGBRGGBRGGB"))
        {
            bayerPattern = Private::Standard;
            filter       = 1;
        }
        else if (identify->filterPattern == QLatin1String("BGGRBGGRBGGRBGGR"))
        {
            bayerPattern = Private::Standard;
            filter       = 2;
        }
        else if (identify->filterPattern == QLatin1String("GBRGGBRGGBRGGBRG"))
        {
            bayerPattern = Private::Standard;
            filter       = 3;
        }
        else if ((identify->filterPattern      == QLatin1String("RGBGRGBGRGBGRGBG")) &&
                 (identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            bayerPattern = Private::Fuji;
            fujiRotate90 = false;
            filter       = 0;
        }
        else if ((identify->filterPattern      == QLatin1String("RBGGBRGGRBGGBRGG")) &&
                 (identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            bayerPattern = Private::Fuji;
            fujiRotate90 = true;
            filter       = 0;
        }
        else if ((identify->filterPattern      == QLatin1String("GGGGBRGGGGRBGGGG")) &&
                 (identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            bayerPattern = Private::Fuji6x6;
            fujiRotate90 = false;
            filter       = 1;
        }
        else if ((identify->rawColors == 3)                 &&
                 (identify->filterPattern.isEmpty())        &&
/*
                 (identify->filterPattern == QString(""))   &&
*/
                 ((uint32)rawData.size() == identify->outputSize.width() * identify->outputSize.height() * 3 * sizeof(uint16)))
        {
            bayerPattern = Private::LinearRaw;
        }
        else if (identify->rawColors == 4)           // Four color sensors
        {
            bayerPattern = Private::FourColor;

            if (identify->filterPattern.length() != 16)
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;

                return FILE_NOT_SUPPORTED;
            }

            for (int i = 0 ; i < 16 ; ++i)
            {
                filter = filter >> 2;

                if      (identify->filterPattern[i] == QLatin1Char('G'))
                {
                    filter |= 0x00000000;
                }
                else if (identify->filterPattern[i] == QLatin1Char('M'))
                {
                    filter |= 0x40000000;
                }
                else if (identify->filterPattern[i] == QLatin1Char('C'))
                {
                    filter |= 0x80000000;
                }
                else if (identify->filterPattern[i] == QLatin1Char('Y'))
                {
                    filter |= 0xC0000000;
                }
                else
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;

                    return FILE_NOT_SUPPORTED;
                }
            }
        }
        else
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;

            return FILE_NOT_SUPPORTED;
        }

        if (fujiRotate90)
        {
            if (!d->fujiRotate(rawData, *identify))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Can not rotate fuji image. Aborted...";

                return PROCESS_FAILED;
            }

            int tmp      = outputWidth;
            outputWidth  = outputHeight;
            outputHeight = tmp;
        }

        activeArea   = dng_rect(identify->outputSize.height(), identify->outputSize.width());
        activeWidth  = identify->outputSize.width();
        activeHeight = identify->outputSize.height();

        // Check if number of Raw Color components is supported.

        if ((identify->rawColors != 3) && (identify->rawColors != 4))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Number of Raw color components not supported. Aborted..." ;

            return PROCESS_FAILED;
        }

        int width  = identify->outputSize.width();
        int height = identify->outputSize.height();
/*
        debugExtractedRAWData(rawData);
*/
        // -----------------------------------------------------------------------------------------

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG memory allocation and initialization" ;

        dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);

        dng_rect rect(height, width);
        DNGWriterHost host(d, &memalloc);

        host.SetSaveDNGVersion(dngVersion_SaveDefault);
        host.SetSaveLinearDNG(false);
        host.SetKeepOriginalFile(true);

        AutoPtr<dng_image> image(new dng_simple_image(rect, (bayerPattern == Private::LinearRaw) ? 3 : 1, ttShort, memalloc));

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        dng_pixel_buffer buffer;

        buffer.fArea       = rect;
        buffer.fPlane      = 0;
        buffer.fPlanes     = (bayerPattern == Private::LinearRaw) ? 3 : 1;
        buffer.fRowStep    = buffer.fPlanes * width;
        buffer.fColStep    = buffer.fPlanes;
        buffer.fPlaneStep  = 1;
        buffer.fPixelType  = ttShort;
        buffer.fPixelSize  = TagTypeSize(ttShort);
        buffer.fData       = rawData.data();
        image->Put(buffer);

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG Negative structure creation";

        AutoPtr<dng_negative> negative(host.Make_dng_negative());

        negative->SetDefaultScale(dng_urational(outputWidth,  activeWidth),
                                  dng_urational(outputHeight, activeHeight));

        if (bayerPattern != Private::LinearRaw)
        {
            negative->SetDefaultCropOrigin(8, 8);
            negative->SetDefaultCropSize(activeWidth - 16, activeHeight - 16);
        }
        else
        {
            negative->SetDefaultCropOrigin(0, 0);
            negative->SetDefaultCropSize(activeWidth, activeHeight);
        }

        negative->SetActiveArea(activeArea);
        negative->SetModelName(identify->model.toLatin1().constData());
        negative->SetLocalName(QString::fromUtf8("%1 %2").arg(identify->make, identify->model).toLatin1().constData());
        negative->SetOriginalRawFileName(d->inputInfo.fileName().toLatin1().constData());
        negative->SetColorChannels(identify->rawColors);

        ColorKeyCode colorCodes[4] =
        {
            colorKeyMaxEnum,
            colorKeyMaxEnum,
            colorKeyMaxEnum,
            colorKeyMaxEnum
        };

        for (int i = 0 ; i < qMax(4, identify->colorKeys.length()) ; ++i)
        {
            if      (identify->colorKeys[i] == QLatin1Char('R'))
            {
                colorCodes[i] = colorKeyRed;
            }
            else if (identify->colorKeys[i] == QLatin1Char('G'))
            {
                colorCodes[i] = colorKeyGreen;
            }
            else if (identify->colorKeys[i] == QLatin1Char('B'))
            {
                colorCodes[i] = colorKeyBlue;
            }
            else if (identify->colorKeys[i] == QLatin1Char('C'))
            {
                colorCodes[i] = colorKeyCyan;
            }
            else if (identify->colorKeys[i] == QLatin1Char('M'))
            {
                colorCodes[i] = colorKeyMagenta;
            }
            else if (identify->colorKeys[i] == QLatin1Char('Y'))
            {
                colorCodes[i] = colorKeyYellow;
            }
        }

        negative->SetColorKeys(colorCodes[0], colorCodes[1], colorCodes[2], colorCodes[3]);

        switch (bayerPattern)
        {
            case Private::Standard:
            {
                // Standard bayer mosaicing. All work fine there.
                // Bayer CCD mask: https://en.wikipedia.org/wiki/Bayer_filter

                negative->SetBayerMosaic(filter);
                break;
            }

            case Private::Fuji:
            {
                // TODO: Fuji is special case. Need to setup different bayer rules here.
                // It do not work in all settings. Need indeep investiguations.
                // Fuji superCCD: https://en.wikipedia.org/wiki/Super_CCD

                negative->SetFujiMosaic(filter);
                break;
            }

            case Private::Fuji6x6:
            {
                // TODO: Fuji is special case. Need to setup different bayer rules here.
                // It do not work in all settings. Need indeep investiguations.

                negative->SetFujiMosaic6x6(filter);
                break;
            }

            case Private::FourColor:
            {
                negative->SetQuadMosaic(filter);
                break;
            }

            default:
            {
                break;
            }
        }

        negative->SetWhiteLevel(identify->whitePoint, 0);
        negative->SetWhiteLevel(identify->whitePoint, 1);
        negative->SetWhiteLevel(identify->whitePoint, 2);
        negative->SetWhiteLevel(identify->whitePoint, 3);

        const dng_mosaic_info* const mosaicinfo = negative->GetMosaicInfo();

        if ((mosaicinfo != nullptr) && (mosaicinfo->fCFAPatternSize == dng_point(2, 2)))
        {
            negative->SetQuadBlacks(identify->blackPoint + identify->blackPointCh[0],
                                    identify->blackPoint + identify->blackPointCh[1],
                                    identify->blackPoint + identify->blackPointCh[2],
                                    identify->blackPoint + identify->blackPointCh[3]);
        }
        else
        {
            negative->SetBlackLevel(identify->blackPoint, 0);
        }

        negative->SetBaselineExposure(0.0);
        negative->SetBaselineNoise(1.0);
        negative->SetBaselineSharpness(1.0);

        dng_orientation orientation;

        switch (identify->orientation)
        {
            case DRawInfo::ORIENTATION_180:
            {
                orientation = dng_orientation::Rotate180();
                break;
            }

            case DRawInfo::ORIENTATION_Mirror90CCW:
            {
                orientation = dng_orientation::Mirror90CCW();
                break;
            }

            case DRawInfo::ORIENTATION_90CCW:
            {
                orientation = dng_orientation::Rotate90CCW();
                break;
            }

            case DRawInfo::ORIENTATION_90CW:
            {
                orientation = dng_orientation::Rotate90CW();
                break;
            }

            default:   // ORIENTATION_NONE
            {
                orientation = dng_orientation::Normal();
                break;
            }
        }

        negative->SetBaseOrientation(orientation);
        negative->SetAntiAliasStrength(dng_urational(100, 100));
        negative->SetLinearResponseLimit(1.0);
        negative->SetShadowScale( dng_urational(1, 1) );
        negative->SetAnalogBalance(dng_vector_3(1.0, 1.0, 1.0));

        // -------------------------------------------------------------------------------

        AutoPtr<dng_camera_profile> prof(new dng_camera_profile);
        prof->SetName(QString::fromUtf8("%1 %2").arg(identify->make, identify->model).toLatin1().constData());

        // Set Camera->XYZ Color matrix as profile.

        dng_matrix matrix;

        switch (identify->rawColors)
        {
            case 3:
            {
                dng_matrix_3by3 camXYZ;
                camXYZ[0][0] = identify->cameraXYZMatrix[0][0];
                camXYZ[0][1] = identify->cameraXYZMatrix[0][1];
                camXYZ[0][2] = identify->cameraXYZMatrix[0][2];
                camXYZ[1][0] = identify->cameraXYZMatrix[1][0];
                camXYZ[1][1] = identify->cameraXYZMatrix[1][1];
                camXYZ[1][2] = identify->cameraXYZMatrix[1][2];
                camXYZ[2][0] = identify->cameraXYZMatrix[2][0];
                camXYZ[2][1] = identify->cameraXYZMatrix[2][1];
                camXYZ[2][2] = identify->cameraXYZMatrix[2][2];

                if (camXYZ.MaxEntry() == 0.0)
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: camera XYZ Matrix is null : camera not supported" ;
                    return FILE_NOT_SUPPORTED;
                }

                matrix = camXYZ;

                break;
            }

            case 4:
            {
                dng_matrix_4by3 camXYZ;
                camXYZ[0][0] = identify->cameraXYZMatrix[0][0];
                camXYZ[0][1] = identify->cameraXYZMatrix[0][1];
                camXYZ[0][2] = identify->cameraXYZMatrix[0][2];
                camXYZ[1][0] = identify->cameraXYZMatrix[1][0];
                camXYZ[1][1] = identify->cameraXYZMatrix[1][1];
                camXYZ[1][2] = identify->cameraXYZMatrix[1][2];
                camXYZ[2][0] = identify->cameraXYZMatrix[2][0];
                camXYZ[2][1] = identify->cameraXYZMatrix[2][1];
                camXYZ[2][2] = identify->cameraXYZMatrix[2][2];
                camXYZ[3][0] = identify->cameraXYZMatrix[3][0];
                camXYZ[3][1] = identify->cameraXYZMatrix[3][1];
                camXYZ[3][2] = identify->cameraXYZMatrix[3][2];

                if (camXYZ.MaxEntry() == 0.0)
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: camera XYZ Matrix is null : camera not supported" ;
                    return FILE_NOT_SUPPORTED;
                }

                matrix = camXYZ;

                break;
            }
        }

        prof->SetColorMatrix1((dng_matrix) matrix);
        prof->SetCalibrationIlluminant1(lsD65);
        negative->AddProfile(prof);

        dng_vector camNeutral(identify->rawColors);

        for (int i = 0 ; i < identify->rawColors ; ++i)
        {
            camNeutral[i] = 1.0 / identify->cameraMult[i];
        }

        negative->SetCameraNeutral(camNeutral);

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->storeMetadata(host, negative, identify.get(), identifyMake.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->backupRaw(host, negative);

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Build DNG Negative" ;

        // Assign Raw image data.

        negative->SetStage1Image(image);

        // Compute linearized and range mapped image

        negative->BuildStage2Image(host);

        // Compute demosaiced image (used by preview and thumbnail)

        negative->BuildStage3Image(host);

        negative->SynchronizeMetadata();
        negative->RebuildIPTC(true);

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        dng_preview_list previewList;
        dng_render negRender(host, *negative.Get());
        dng_jpeg_preview* jpeg_preview = new dng_jpeg_preview();

        if (d->previewMode != DNGWriter::NONE)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG preview image creation" ;

            dng_jpeg_preview* jpeg_preview  = new dng_jpeg_preview();
            jpeg_preview->fInfo.fApplicationName.Set_ASCII(QString::fromLatin1("digiKam").toLatin1().constData());
            jpeg_preview->fInfo.fApplicationVersion.Set_ASCII(digiKamVersion().toLatin1().constData());
            jpeg_preview->fInfo.fDateTime   = d->orgDateTimeInfo.Encode_ISO_8601();
            jpeg_preview->fInfo.fColorSpace = previewColorSpace_sRGB;

            negRender.SetMaximumSize(d->previewMode == MEDIUM ? 1280 : width);
            AutoPtr<dng_image> negImage(negRender.Render());
            dng_image_writer jpegWriter;
            jpegWriter.EncodeJPEGPreview(host, *negImage.Get(), *jpeg_preview, 5);
            AutoPtr<dng_preview> jp(static_cast<dng_preview*>(jpeg_preview));
            previewList.Append(jp);
        }

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG thumbnail creation" ;

        dng_image_preview* thumbnail         = new dng_image_preview();
        thumbnail->fInfo.fApplicationName    = jpeg_preview->fInfo.fApplicationName;
        thumbnail->fInfo.fApplicationVersion = jpeg_preview->fInfo.fApplicationVersion;
        thumbnail->fInfo.fDateTime           = jpeg_preview->fInfo.fDateTime;
        thumbnail->fInfo.fColorSpace         = jpeg_preview->fInfo.fColorSpace;

        negRender.SetMaximumSize(256);
        thumbnail->fImage.Reset(negRender.Render());
        AutoPtr<dng_preview> tn(static_cast<dng_preview*>(thumbnail));
        previewList.Append(tn);

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Creating DNG file " << outputInfo.fileName() ;

        dng_image_writer writer;
        dng_file_stream filestream(QFile::encodeName(dngFilePath).constData(), true);

        writer.WriteDNG(host,
                        filestream,
                        *negative.Get(),
                        &previewList,
                        dngVersion_SaveDefault,
                        !d->jpegLossLessCompression
                       );

        // -----------------------------------------------------------------------------------------
        // Metadata makernote cleanup using Exiv2 for some RAW file types
        // See bug #204437 and #210371, and write XMP Sidecar if necessary
        // We disable writing to RAW files, see bug #381432
/*
        if (meta->load(dngFilePath))
        {
            if (d->inputInfo.suffix().toUpper() == QLatin1String("ORF"))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: cleanup makernotes using Exiv2" ;

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

        if (d->updateFileDate)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Setting modification date from meta data: " << d->fileDate.toString();

            DFileOperations::setModificationTime(dngFilePath, d->fileDate);
        }
    }

    catch (const dng_exception& exception)
    {
        int ret = exception.ErrorCode();
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code (" << ret << "):" << d->dngErrorCodeToString(ret);

        return DNG_SDK_INTERNAL_ERROR;
    }

    catch (...)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code unknow";

        return DNG_SDK_INTERNAL_ERROR;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG conversion complete...";

    return PROCESS_COMPLETE;
}

} // namespace Digikam
