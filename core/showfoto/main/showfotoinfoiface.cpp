/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : interface to item information for Showfoto
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "showfotoinfoiface.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "showfotosetup.h"
#include "digikam_debug.h"

namespace ShowFoto
{

ShowfotoInfoIface::ShowfotoInfoIface(QObject* const parent, const QList<QUrl>& lst)
    : DMetaInfoIface(parent, lst)
{
}

ShowfotoInfoIface::~ShowfotoInfoIface()
{
}

void ShowfotoInfoIface::openSetupPage(SetupPage page)
{
    switch (page)
    {
        case ExifToolPage:
        {
            if (ShowfotoSetup::execExifTool(nullptr))
            {
                emit signalSetupChanged();
            }
        }

        default:
        {
        }
    }
}

} // namespace ShowFoto
