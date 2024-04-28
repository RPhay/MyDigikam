/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-06-14
 * Description : A JPEG-2000 IO file for DImg framework
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcpp"
#endif

#if defined(Q_OS_DARWIN) && defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wshift-negative-value"
#endif

#include <jasper/jasper.h>
#include <jasper/jas_version.h>

// Restore warnings
#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

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

#if defined JAS_VERSION_MAJOR && JAS_VERSION_MAJOR >= 3

    return jas_init_thread();

#else

    return jas_init();

#endif

}

void DImgJPEG2000Loader::cleanupJasper()
{

#if defined JAS_VERSION_MAJOR && JAS_VERSION_MAJOR >= 3

    jas_cleanup_thread();

#else

    jas_cleanup();

#endif

}

} // namespace DigikamJPEG2000DImgPlugin
