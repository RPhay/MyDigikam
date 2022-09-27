/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-24-01
 * Description : stretch contrast image filter.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_STRETCH_FILTER_H
#define DIGIKAM_STRETCH_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT StretchFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit StretchFilter(QObject* const parent = nullptr);
    StretchFilter(DImg* const orgImage, const DImg* const refImage, QObject* const parent = nullptr);
    ~StretchFilter()                                                          override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:StretchFilter");
    }

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    static QString DisplayableName();

    QString         filterIdentifier()                                  const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                            override;

    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;
    void stretchContrastImage();

private:

    struct Q_DECL_HIDDEN double_packet
    {
        double_packet()
            : red  (0.0),
              green(0.0),
              blue (0.0),
              alpha(0.0)
        {
        }

        double red;
        double green;
        double blue;
        double alpha;
    };

    struct Q_DECL_HIDDEN int_packet
    {
       int_packet()
            : red  (0),
              green(0),
              blue (0),
              alpha(0)
        {
        }

        unsigned int red;
        unsigned int green;
        unsigned int blue;
        unsigned int alpha;
    };

    DImg m_refImage;
};

} // namespace Digikam

#endif // DIGIKAM_STRETCH_FILTER_H
