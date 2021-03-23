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

#ifndef DIGIKAM_DIMG_PNG_LOADER_H
#define DIGIKAM_DIMG_PNG_LOADER_H

// C++ includes

#include <cstdarg>

// Local includes

#include "dimg.h"
#include "dimgloader.h"

using namespace Digikam;

namespace DigikamPNGDImgPlugin
{

class DImgPNGLoader : public DImgLoader
{
public:

    explicit DImgPNGLoader(DImg* const image);
    ~DImgPNGLoader()                                                       override;

    bool load(const QString& filePath, DImgLoaderObserver* const observer) override;
    bool save(const QString& filePath, DImgLoaderObserver* const observer) override;

    bool hasAlpha()                                                  const override;
    bool sixteenBit()                                                const override;
    bool isReadOnly()                                                const override;

private:

    bool m_sixteenBit;
    bool m_hasAlpha;
};

} // namespace DigikamPNGDImgPlugin

#endif // DIGIKAM_DIMG_PNG_LOADER_H
