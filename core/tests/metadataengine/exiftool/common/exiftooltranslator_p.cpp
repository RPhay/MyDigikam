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

class Q_DECL_HIDDEN ExifToolTranslatorCreator
{
public:

    ExifToolTranslator object;
};

Q_GLOBAL_STATIC(ExifToolTranslatorCreator, creator)

ExifToolTranslator* ExifToolTranslator::instance()
{
    return &creator->object;
}

// --------------------------------------------------------------------------

ExifToolTranslator::Private::Private()
{
    // --- Create the hash-table of tag names (ExifTool to Exiv2).

    populateExifHashTable();
    populateIptcHashTable();
    populateXmpHashTable();

    // --- Create the ignred list of ExiTool tag names.

    populateIgnoredGroups();

    // --- Create the reverse hash-table of tag names (Exiv2 to ExifTool).

    QHash<QString, QString>::const_iterator it = mapETtoExiv2.constBegin();

    while (it != mapETtoExiv2.constEnd())
    {
        if (it.value() != QLatin1String("..."))           // Ignore Exiv2 tags not yet defined
        {
            mapExiv2toET.insert(it.value(), it.key());
        }

        ++it;
    }
}

ExifToolTranslator::Private::~Private()
{
}

} // namespace Digikam
