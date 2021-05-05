/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-11-01
 * Description : a PNG image loader for DImg framework.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define PNG_BYTES_TO_CHECK 4

#include "dimgpngloader.h"

// C ANSI includes

extern "C"
{
#ifndef Q_CC_MSVC
#   include <unistd.h>
#endif
}

// C++ includes

#include <cstdlib>
#include <cstdio>

// Qt includes

#include <QFile>
#include <QByteArray>
#include <QSysInfo>

// Local includes

#include "metaengine.h"
#include "digikam_debug.h"
#include "digikam_config.h"
#include "digikam_version.h"
#include "dimgloaderobserver.h"

// libPNG includes

extern "C"
{
#include <png.h>
}

using namespace Digikam;

namespace DigikamPNGDImgPlugin
{

DImgPNGLoader::DImgPNGLoader(DImg* const image)
    : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

DImgPNGLoader::~DImgPNGLoader()
{
}

bool DImgPNGLoader::hasAlpha() const
{
    return m_hasAlpha;
}

bool DImgPNGLoader::sixteenBit() const
{
    return m_sixteenBit;
}

bool DImgPNGLoader::isReadOnly() const
{
    return false;
}

} // namespace DigikamPNGDImgPlugin
