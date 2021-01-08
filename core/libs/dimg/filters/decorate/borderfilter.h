/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : border threaded image filter.
 *
 * Copyright 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_BORDER_FILTER_H
#define DIGIKAM_BORDER_FILTER_H

// Qt includes

#include <QImage>

// Local includes

#include "dimgthreadedfilter.h"
#include "digikam_globals.h"
#include "bordercontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT BorderFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    /**
     * Constructor using settings to preserve aspect ratio of image.
     */
    explicit BorderFilter(QObject* const parent = nullptr);
    explicit BorderFilter(DImg* orgImage, QObject* const parent = nullptr, const BorderContainer& settings = BorderContainer());
    ~BorderFilter()                                           override;

    static QString FilterIdentifier()
    {
        return QLatin1String("digikam:BorderFilter");
    }

    static QString DisplayableName();

    static QList<int> SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int CurrentVersion()
    {
        return 1;
    }

    QString filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction filterAction()                               override;
    void readParameters(const FilterAction& action)           override;

private:

    void filterImage()                                        override;

    /**
     * Methods to preserve aspect ratio of image.
     */
    void solid(DImg& src, DImg& dest, const DColor& fg, int borderWidth);
    void niepce(DImg& src, DImg& dest, const DColor& fg, int borderWidth,
                const DColor& bg, int lineWidth);
    void bevel(DImg& src, DImg& dest, const DColor& topColor,
               const DColor& btmColor, int borderWidth);
    void pattern(DImg& src, DImg& dest, int borderWidth, const DColor& firstColor,
                 const DColor& secondColor, int firstWidth, int secondWidth);

    /**
     * Methods to not-preserve aspect ratio of image.
     */
    void solid2(DImg& src, DImg& dest, const DColor& fg, int borderWidth);
    void niepce2(DImg& src, DImg& dest, const DColor& fg, int borderWidth,
                 const DColor& bg, int lineWidth);
    void bevel2(DImg& src, DImg& dest, const DColor& topColor,
                const DColor& btmColor, int borderWidth);
    void pattern2(DImg& src, DImg& dest, int borderWidth, const DColor& firstColor,
                  const DColor& secondColor, int firstWidth, int secondWidth);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BORDER_FILTER_H
