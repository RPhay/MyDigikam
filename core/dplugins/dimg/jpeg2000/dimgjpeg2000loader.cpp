/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-06-14
 * Description : A JPEG-2000 IO file for DImg framework
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dimgjpeg2000loader.h"

// Local includes

#include "digikam_debug.h"

// Jasper includes

#ifndef Q_CC_MSVC
extern "C"
{
#endif

#if defined(Q_OS_DARWIN) && defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wshift-negative-value"
#endif

#include <jasper/jasper.h>
#include <jasper/jas_version.h>

#if defined(Q_OS_DARWIN) && defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

#ifndef Q_CC_MSVC
}
#endif

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

int DImgJPEG2000Loader::initJasper()
{

#if JAS_VERSION_MAJOR < 3

    return jas_init();

#else

    return jas_init_thread();

#endif

}

void DImgJPEG2000Loader::cleanupJasper()
{

#if JAS_VERSION_MAJOR < 3

    jas_cleanup();

#else

    jas_cleanup_thread();

#endif

}

} // namespace DigikamJPEG2000DImgPlugin
