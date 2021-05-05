/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : A QImage loader for DImg framework.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2019 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DIMG_QIMAGE_LOADER_H
#define DIGIKAM_DIMG_QIMAGE_LOADER_H

// Local includes

#include "dimg.h"
#include "dimgloader.h"
#include "digikam_config.h"

using namespace Digikam;

namespace DigikamQImageDImgPlugin
{

class DImgQImageLoader : public DImgLoader
{
public:

    explicit DImgQImageLoader(DImg* const image);
    ~DImgQImageLoader()                                                    override;

    bool load(const QString& filePath, DImgLoaderObserver* const observer) override;
    bool save(const QString& filePath, DImgLoaderObserver* const observer) override;

    bool hasAlpha()                                                  const override;
    bool sixteenBit()                                                const override;
    bool isReadOnly()                                                const override;

private:

    bool m_hasAlpha;
    bool m_sixteenBit;
};

} // namespace DigikamQImageDImgPlugin

#endif // DIGIKAM_DIMG_QIMAGE_LOADER_H
