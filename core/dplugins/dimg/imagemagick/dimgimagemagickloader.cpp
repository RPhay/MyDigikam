/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-19
 * Description : ImageMagick loader for DImg framework.
 *
 * Copyright (C) 2019 by Caulier Gilles <caulier dot gilles at gmail dot com>
 * Copyright (c) 2019 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "dimgimagemagickloader.h"

// ImageMagick includes

// Pragma directives to reduce warnings from ImageMagick header files.
#if defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wkeyword-macro"
#endif

#include <Magick++.h>

// Restore warnings

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

#if defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

// Local includes

#include "digikam_debug.h"
#include "dimgloaderobserver.h"

using namespace Magick;

namespace DigikamImageMagickDImgPlugin
{

DImgImageMagickLoader::DImgImageMagickLoader(DImg* const image)
    : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

DImgImageMagickLoader::~DImgImageMagickLoader()
{
}

bool DImgImageMagickLoader::hasAlpha() const
{
    return m_hasAlpha;
}

bool DImgImageMagickLoader::sixteenBit() const
{
    return m_sixteenBit;
}

bool DImgImageMagickLoader::isReadOnly() const
{
    return false;
}

} // namespace DigikamImageMagickDImgPlugin
