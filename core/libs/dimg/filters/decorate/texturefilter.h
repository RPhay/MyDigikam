/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : TextureFilter threaded image filter.
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

#ifndef DIGIKAM_TEXTURE_FILTER_H
#define DIGIKAM_TEXTURE_FILTER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "texturecontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT TextureFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit TextureFilter(QObject* const parent = nullptr);
    explicit TextureFilter(DImg* const orgImage,
                           QObject* const parent = nullptr,
                           const TextureContainer& settings = TextureContainer());

    ~TextureFilter() override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:TextureFilter");
    }

    static QString          DisplayableName();

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 2;
    }

    static int              CurrentVersion()
    {
        return 2;
    }

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;
    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;

private:

    TextureContainer m_settings;
};

} // namespace Digikam

#endif // DIGIKAM_TEXTURE_FILTER_H
