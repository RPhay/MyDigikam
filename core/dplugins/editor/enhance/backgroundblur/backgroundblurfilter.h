/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-08-24
 * Description : image editor plugin to blur the background of an image
 *
 * SPDX-FileCopyrightText: 2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

using namespace Digikam;

namespace DigikamEditorBackgroundBlurToolPlugin
{

class DIGIKAM_EXPORT BackgroundBlurFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit BackgroundBlurFilter(QObject* const parent = nullptr);
    explicit BackgroundBlurFilter(DImg* const orgImage,
                                  const QRect& selection,
                                  int radius = 3,
                                  QObject* const parent = nullptr);

    /**
     * Constructor for slave mode: execute immediately in current thread with specified master filter
     */
    explicit BackgroundBlurFilter(DImgThreadedFilter* const parentFilter,
                                  const DImg& orgImage,
                                  const DImg& destImage,
                                  const QRect& selection,
                                  int radius = 3,
                                  int progressBegin = 0,
                                  int progressEnd = 100);

    ~BackgroundBlurFilter()                                                   override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:BackgroundBackgroundBlurFilter");
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

    QString         filterIdentifier()                                  const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                            override;

    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamEditorBackgroundBlurToolPlugin
