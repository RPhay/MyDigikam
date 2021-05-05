/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect align_image_stack binary program and version
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "alignbinary.h"

namespace DigikamGenericExpoBlendingPlugin
{

AlignBinary::AlignBinary()
    : DBinaryIface(QLatin1String("align_image_stack"),
                   QLatin1String("0.8"),
                   QLatin1String("align_image_stack version "),
                   1,
                   QLatin1String("Hugin"),
                   QLatin1String("http://hugin.sourceforge.net/download/"),     // krazy:exclude=insecurenet
                   QLatin1String("ExpoBlending"),
                   QStringList(QLatin1String("-h"))
                  )
{
    setup();
}

AlignBinary::~AlignBinary()
{
}

} // namespace DigikamGenericExpoBlendingPlugin
