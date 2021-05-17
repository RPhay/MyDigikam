/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Xmp Metadata storage.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

int DNGWriter::Private::storeXmp(DNGWriterHost& host,
                                 AutoPtr<dng_negative>& negative,
                                 DRawInfo* const /*identify*/,
                                 DRawInfo* const /*identifyMake*/,
                                 DMetadata* const /*meta*/)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Updating Xmp metadata to DNG Negative";

    dng_date_time_info dateTimeNow;
    CurrentDateTimeAndZone(dateTimeNow);

    AutoPtr<dng_xmp> negXmp(new dng_xmp(host.Allocator()));

    negXmp->UpdateDateTime(dateTimeNow);
    negXmp->UpdateMetadataDate(dateTimeNow);
    negXmp->Set(XMP_NS_DC, "format", "image/dng");

    if (metaLoaded)
    {
        negXmp->SetString(XMP_NS_XAP,       "CreatorTool", negative->GetExif()->fSoftware);
        negXmp->SetString(XMP_NS_PHOTOSHOP, "DateCreated", negative->GetExif()->fDateTimeOriginal.Encode_ISO_8601());
    }

    negative->ResetXMP(negXmp.Release());

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
