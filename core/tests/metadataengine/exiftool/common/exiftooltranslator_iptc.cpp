/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-13
 * Description : ExifTool tags translator for Exiv2
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftooltranslator_p.h"

namespace Digikam
{

void ExifToolTranslator::Private::populateIptcHashTable()
{
    // --- IPTC Table -------------------------------------------------------------------

    // Iptc Envelope Group

    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.CodedCharacterSet"),            QLatin1String("Iptc.Envelope.CharacterSet"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.EnvelopeRecordVersion"),        QLatin1String("Iptc.Envelope.ModelVersion"));

    // Iptc Application2 Group

    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ApplicationRecordVersion"),     QLatin1String("Iptc.Application2.RecordVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Caption-Abstract"),             QLatin1String("Iptc.Application2.Caption"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.OriginatingProgram"),           QLatin1String("Iptc.Application2.Program"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ProgramVersion"),               QLatin1String("Iptc.Application2.ProgramVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Keywords"),                     QLatin1String("Iptc.Application2.Keywords"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ObjectName"),                   QLatin1String("Iptc.Application2.ObjectName"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.By-line"),                     QLatin1String("Iptc.Application2.Byline"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.CopyrightNotice"),             QLatin1String("Iptc.Application2.Copyright"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DateCreated"),                   QLatin1String("Iptc.Application2.DateCreated"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.TimeCreated"),                   QLatin1String("Iptc.Application2.TimeCreated"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DigitalCreationDate"),           QLatin1String("Iptc.Application2.DigitizationDate"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DigitalCreationTime"),           QLatin1String("Iptc.Application2.DigitizationTime"));
}

} // namespace Digikam
