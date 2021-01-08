/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : HotPixel properties container.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef DIGIKAM_HOT_PIXEL_PROPS_H
#define DIGIKAM_HOT_PIXEL_PROPS_H

// Qt includes

#include <QRect>
#include <QList>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT HotPixelProps
{

public:

    QRect rect;
    int   luminosity;

public:

    int y()                                                     const;
    int x()                                                     const;
    int width()                                                 const;
    int height()                                                const;

    bool operator==(const HotPixelProps& p)                     const;

    // Helper methods to stream container data with string.
    QString toString()                                          const;
    bool fromString(const QString& str);

public:

    // Helper methods to stream list of containers data with string list.
    static QStringList toStringList(const QList<HotPixelProps>& lst);
    static QList<HotPixelProps> fromStringList(const QStringList& hplst);

private:

    bool diagonal(const QRect& r1, const QRect& r2)             const;
};

} // namespace Digikam

#endif // DIGIKAM_HOT_PIXEL_PROPS_H
