/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-03-30
 * Description : abstract class to host DImg loader settings.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimgloadersettings.h"

namespace Digikam
{

DImgLoaderSettings::DImgLoaderSettings(QWidget* const parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

DImgLoaderSettings::~DImgLoaderSettings()
{
}

} // namespace Digikam
