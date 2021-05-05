/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : Oil Painting threaded image filter.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_OIL_PAINT_FILTER_H
#define DIGIKAM_OIL_PAINT_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DIGIKAM_EXPORT OilPaintFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit OilPaintFilter(QObject* const parent = nullptr);
    explicit OilPaintFilter(DImg* const orgImage, QObject* const parent = nullptr, int brushSize=1, int smoothness=30);
    ~OilPaintFilter() override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:OilPaintFilter");
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

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;
    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;
    void oilPaintImageMultithreaded(uint start, uint stop);
    DColor MostFrequentColor(DImg& src, int X, int Y, int Radius, int Intensity,
                             uchar* intensityCount, uint* averageColorR, uint* averageColorG, uint* averageColorB);
    inline double GetIntensity(uint Red, uint Green, uint Blue);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_OIL_PAINT_FILTER_H
