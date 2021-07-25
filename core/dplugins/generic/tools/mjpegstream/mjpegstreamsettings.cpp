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
    : m_port          (8080),
      m_loop          (true),
      m_quality       (75),
      m_interval      (5)
{
}

MjpegStreamSettings::~MjpegStreamSettings()
{
}

void MjpegStreamSettings::readSettings(KConfigGroup& group)
{
    m_port     = group.readEntry("MJPEGStreamPort",     8080);
    m_loop     = group.readEntry("MJPEGStreamLoop",     true);
    m_quality  = group.readEntry("MJPEGStreamQuality",  75);
    m_interval = group.readEntry("MJPEGStreamInterval", 5);
}

void MjpegStreamSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry("MJPEGStreamPort",     m_port);
    group.writeEntry("MJPEGStreamLoop",     m_loop);
    group.writeEntry("MJPEGStreamQuality",  m_quality);
    group.writeEntry("MJPEGStreamInterval", m_interval);
}

} // namespace DigikamGenericMjpegStreamPlugin
