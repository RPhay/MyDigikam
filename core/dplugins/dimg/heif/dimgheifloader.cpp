/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-26
 * Description : A HEIF IO file for DImg framework
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimgheifloader.h"

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "dimgloaderobserver.h"

namespace Digikam
{

DImgHEIFLoader::DImgHEIFLoader(DImg* const image)
    : DImgLoader(image)
{
    m_hasAlpha           = false;
    m_sixteenBit         = false;
    m_observer           = nullptr;
    m_maxOutputBitsDepth = -1;
    m_quality            = 50;
    m_lossless           = false;
}

bool DImgHEIFLoader::hasAlpha() const
{
    return m_hasAlpha;
}

bool DImgHEIFLoader::sixteenBit() const
{
    return m_sixteenBit;
}

bool DImgHEIFLoader::isReadOnly() const
{
#ifdef HAVE_X265
    return false;
#else
    return true;
#endif
}

bool DImgHEIFLoader::isHeifSuccess(struct heif_error* const error)
{
    if (error->code == 0)
    {
        return true;
    }

    qWarning() << "Error while processing HEIC image:" << error->message;
    return false;
}

} // namespace Digikam
