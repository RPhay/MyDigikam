/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : Threaded image filter to fix hot pixels
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef DIGIKAM_HOT_PIXEL_FIXER_H
#define DIGIKAM_HOT_PIXEL_FIXER_H

// Qt includes

#include <QList>
#include <QImage>
#include <QObject>
#include <QRect>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "hotpixelprops.h"
#include "hotpixelsweights.h"
#include "hotpixelcontainer.h"

using namespace Digikam;

namespace Digikam
{

class DIGIKAM_EXPORT HotPixelFixer : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit HotPixelFixer(QObject* const parent = nullptr);
    explicit HotPixelFixer(DImg* const orgImage,
                           QObject* const parent,
                           const HotPixelContainer& settings);
    ~HotPixelFixer() override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:HotPixelFilter");
    }

    static QString          DisplayableName();

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    void readParameters(const FilterAction& action)  override;

    QString         filterIdentifier() const override
    {
        return FilterIdentifier();
    }

    Digikam::FilterAction filterAction()     override;

private:

    void filterImage()                       override;

    void interpolate(DImg& img,
                     HotPixelProps& hp,
                     int method);

    void weightPixels(DImg& img,
                      HotPixelProps& px,
                      int method,
                      HotPixelContainer::Direction dir,
                      int maxComponent);

    inline bool validPoint(DImg& img, const QPoint& p)
    {
        return (
                (p.x() >= 0)                 &&
                (p.y() >= 0)                 &&
                (p.x() < (long) img.width()) &&
                (p.y() < (long) img.height())
               );
    };


private:

    QList<HotPixelsWeights> m_weightList;

    HotPixelContainer       m_settings;
};

} // namespace Digikam

#endif // DIGIKAM_HOT_PIXEL_FIXER_H
