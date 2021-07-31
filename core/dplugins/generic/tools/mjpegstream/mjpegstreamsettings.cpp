/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG settings container.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mjpegstreamsettings.h"

// KDE includes

#include <kconfiggroup.h>

namespace DigikamGenericMjpegStreamPlugin
{

MjpegStreamSettings::MjpegStreamSettings()
    : port    (8080),
      loop    (true),
      quality (75),
      delay   (5),
      iface   (nullptr)
{
}

MjpegStreamSettings::~MjpegStreamSettings()
{
}

void MjpegStreamSettings::setCollectionMap(const MjpegServerMap& map)
{
    urlsList.clear();

    for (MjpegServerMap::const_iterator it = map.constBegin() ; it != map.constEnd() ; ++it)
    {
        urlsList.append(it.value());
    }
}

void MjpegStreamSettings::readSettings(KConfigGroup& group)
{
    port     = group.readEntry("MJPEGStreamPort",     8080);
    loop     = group.readEntry("MJPEGStreamLoop",     true);
    quality  = group.readEntry("MJPEGStreamQuality",  75);
    delay    = group.readEntry("MJPEGStreamDelay",    5);
}

void MjpegStreamSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry("MJPEGStreamPort",     port);
    group.writeEntry("MJPEGStreamLoop",     loop);
    group.writeEntry("MJPEGStreamQuality",  quality);
    group.writeEntry("MJPEGStreamDelay",    delay);
}

} // namespace DigikamGenericMjpegStreamPlugin
