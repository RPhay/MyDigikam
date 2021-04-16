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

#ifndef DIGIKAM_EXIF_TOOL_TRANSLATOR_H
#define DIGIKAM_EXIF_TOOL_TRANSLATOR_H

// Qt includes

#include <QString>

namespace Digikam
{

class ExifToolTranslator
{
public:

    /// This class is a singleton to optimize time loading of intenal hash-tables.
    static ExifToolTranslator* instance();

    QString translateToExiv2(const QString& tagName)    const;
    QString translateToExifTool(const QString& tagName) const;

    bool isIgnoredGroup(const QString& group)           const;

private:

    // Disable
    ExifToolTranslator();
    ~ExifToolTranslator();

    ExifToolTranslator(const ExifToolTranslator&)            = delete;
    ExifToolTranslator& operator=(const ExifToolTranslator&) = delete;

private:

    friend class ExifToolTranslatorCreator;

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_TRANSLATOR_H_
