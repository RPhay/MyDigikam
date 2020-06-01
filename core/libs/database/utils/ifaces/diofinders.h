/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-17
 * Description : low level files management interface - Finder classes.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2018      by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_DIO_FINDERS_H
#define DIGIKAM_DIO_FINDERS_H

// Qt includes

#include <QList>
#include <QUrl>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class ItemInfo;

class DIGIKAM_GUI_EXPORT SidecarFinder
{

public:

    explicit SidecarFinder(const QList<QUrl>& files);

public:

    QList<QUrl>    localFiles;
    QList<bool>    localFileModes;
    QList<QString> localFileSuffixes;
};

// -----------------------------------------------------------------------------------------

class DIGIKAM_GUI_EXPORT GroupedImagesFinder
{

public:

    explicit GroupedImagesFinder(const QList<ItemInfo>& source);

public:

    QList<ItemInfo> infos;
};

} // namespace Digikam

#endif // DIGIKAM_DIO_FINDERS_H
