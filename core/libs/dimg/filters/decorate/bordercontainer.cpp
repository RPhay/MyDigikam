/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : border settings container.
 *
 * Copyright 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#include "bordercontainer.h"

namespace Digikam
{

BorderContainer::BorderContainer()
    : preserveAspectRatio(true),
      orgWidth(0),
      orgHeight(0),
      borderType(0),
      borderWidth1(0),
      borderWidth2(0),
      borderWidth3(0),
      borderWidth4(0),
      borderPercent(0.1),
      solidColor(QColor(0, 0, 0)),
      niepceBorderColor(QColor(255, 255, 255)),
      niepceLineColor(QColor(0, 0, 0)),
      bevelUpperLeftColor(QColor(192, 192, 192)),
      bevelLowerRightColor(QColor(128, 128, 128)),
      decorativeFirstColor(QColor(0, 0, 0)),
      decorativeSecondColor(QColor(0, 0, 0))
{
}

BorderContainer::~BorderContainer()
{
}

} // namespace Digikam
