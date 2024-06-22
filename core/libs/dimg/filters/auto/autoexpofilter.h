/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-15-02
 * Description : auto exposure image filter.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_export.h"
#include "wbfilter.h"
#include "digikam_globals.h"
#include "filteraction.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT AutoExpoFilter : public WBFilter
{
    Q_OBJECT

public:

    explicit AutoExpoFilter(QObject* const parent = nullptr);
    AutoExpoFilter(DImg* const orgImage, const DImg* const refImage, QObject* const parent = nullptr);
    ~AutoExpoFilter()                                                         override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:AutoExpoFilter");
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

private:

    DImg m_refImage;
};

} // namespace Digikam
