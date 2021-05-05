/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-05
 * Description : HotPixel filter settings container.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_HOTPIXEL_CONTAINER_H
#define DIGIKAM_HOTPIXEL_CONTAINER_H

// Qt includes

#include <QString>
#include <QUrl>
#include <QList>

// Local includes

#include "digikam_export.h"
#include "hotpixelprops.h"

namespace Digikam
{

class FilterAction;

class DIGIKAM_EXPORT HotPixelContainer
{

public:

    enum InterpolationMethod
    {
        AVERAGE_INTERPOLATION   = 0,
        LINEAR_INTERPOLATION    = 1,
        QUADRATIC_INTERPOLATION = 2,
        CUBIC_INTERPOLATION     = 3
    };

    enum Direction
    {
        TWODIM_DIRECTION        = 0,
        VERTICAL_DIRECTION      = 1,
        HORIZONTAL_DIRECTION    = 2
    };

public:

    explicit HotPixelContainer();
    ~HotPixelContainer();

    bool isDefault() const;
    bool operator==(const HotPixelContainer& other)                                   const;

    void writeToFilterAction(FilterAction& action, const QString& prefix = QString()) const;

    static HotPixelContainer fromFilterAction(const FilterAction& action,
                                              const QString& prefix = QString());

public:

    QUrl                 blackFrameUrl;
    QList<HotPixelProps> hotPixelsList;
    InterpolationMethod  filterMethod;
};

} // namespace Digikam

#endif // DIGIKAM_HOTPIXEL_CONTAINER_H
