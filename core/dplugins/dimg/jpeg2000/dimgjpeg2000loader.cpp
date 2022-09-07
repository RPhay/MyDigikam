/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-06-14
 * Description : A JPEG-2000 IO file for DImg framework
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dimgjpeg2000loader.h"

// Local includes

#include "digikam_debug.h"

namespace DigikamJPEG2000DImgPlugin
{

DImgJPEG2000Loader::DImgJPEG2000Loader(DImg* const image)
    : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

DImgJPEG2000Loader::~DImgJPEG2000Loader()
{
}

bool DImgJPEG2000Loader::hasAlpha() const
{
    return m_hasAlpha;
}

bool DImgJPEG2000Loader::sixteenBit() const
{
    return m_sixteenBit;
}

bool DImgJPEG2000Loader::isReadOnly() const
{
    return false;
}

} // namespace DigikamJPEG2000DImgPlugin
