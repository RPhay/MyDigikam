/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-19
 * Description : ImageMagick loader for DImg framework.
 *
 * SPDX-FileCopyrightText: 2019 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DIMG_IMAGEMAGICK_LOADER_H
#define DIGIKAM_DIMG_IMAGEMAGICK_LOADER_H

// Local includes

#include "dimg.h"
#include "dimgloader.h"
#include "digikam_config.h"

using namespace Digikam;

namespace DigikamImageMagickDImgPlugin
{

class DImgImageMagickLoader : public DImgLoader
{
public:

    explicit DImgImageMagickLoader(DImg* const image);
    ~DImgImageMagickLoader()                                               override;

    bool load(const QString& filePath, DImgLoaderObserver* const observer) override;
    bool save(const QString& filePath, DImgLoaderObserver* const observer) override;

    bool hasAlpha()                                                  const override;
    bool sixteenBit()                                                const override;
    bool isReadOnly()                                                const override;

private:

    bool m_sixteenBit;
    bool m_hasAlpha;
};

} // namespace DigikamImageMagickDImgPlugin

#endif // DIGIKAM_DIMG_IMAGEMAGICK_LOADER_H
