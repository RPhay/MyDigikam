/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Run Convertion stages.
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
        QScopedPointer<DRawInfo> identify(new DRawInfo);
        QScopedPointer<DRawInfo> identifyMake(new DRawInfo);

        ret = d->importRaw(identify.get(), identifyMake.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->identMosaic(identify.get(), identifyMake.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG memory allocation and initialization" ;

        dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);

        dng_rect rect(d->height, d->width);
        DNGWriterHost host(d, &memalloc);

        host.SetSaveDNGVersion(dngVersion_SaveDefault);
        host.SetSaveLinearDNG(false);
        host.SetKeepOriginalFile(true);

        AutoPtr<dng_image> image(new dng_simple_image(rect, (d->bayerPattern == Private::LinearRaw) ? 3 : 1, ttShort, memalloc));

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        dng_pixel_buffer buffer;

        buffer.fArea       = rect;
        buffer.fPlane      = 0;
        buffer.fPlanes     = (d->bayerPattern == Private::LinearRaw) ? 3 : 1;
        buffer.fRowStep    = buffer.fPlanes * d->width;
        buffer.fColStep    = buffer.fPlanes;
        buffer.fPlaneStep  = 1;
        buffer.fPixelType  = ttShort;
        buffer.fPixelSize  = TagTypeSize(ttShort);
        buffer.fData       = d->rawData.data();
        image->Put(buffer);

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        AutoPtr<dng_negative> negative(host.Make_dng_negative());

        ret = d->createNegative(negative, identify.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        QScopedPointer<DMetadata> meta(new DMetadata);
        d->exif = negative->GetExif();

        ret = d->storeExif(host, negative, identify.get(), identifyMake.get(), meta.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->storeMakernote(host, negative, identify.get(), identifyMake.get(), meta.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->storeXmp(host, negative, identify.get(), identifyMake.get(), meta.get());

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

        ret = d->exportTarget(host, negative, image);

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->exifToolPostProcess();

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }
    }

    catch (const dng_exception& exception)
    {
        int ret = exception.ErrorCode();
        qCWarning(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code (" << ret << "):" << d->dngErrorCodeToString(ret);

        return DNG_SDK_INTERNAL_ERROR;
    }

    catch (...)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code unknow";

        return DNG_SDK_INTERNAL_ERROR;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG conversion complete...";

    return PROCESS_COMPLETE;
}

} // namespace Digikam
