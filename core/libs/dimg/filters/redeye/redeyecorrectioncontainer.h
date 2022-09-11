/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-15
 * Description : Red Eyes auto correction settings container.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2016      by Omar Amin <Omar dot moh dot amin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_RED_EYE_CORRECTION_CONTAINER_H
#define DIGIKAM_RED_EYE_CORRECTION_CONTAINER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class FilterAction;

class DIGIKAM_EXPORT RedEyeCorrectionContainer
{

public:

    explicit RedEyeCorrectionContainer();

    bool isDefault()                                            const;
    bool operator==(const RedEyeCorrectionContainer& other)     const;

    void writeToFilterAction(FilterAction& action,
                             const QString& prefix = QString()) const;

    static RedEyeCorrectionContainer fromFilterAction(const FilterAction& action,
                                                      const QString& prefix = QString());

public:

    double m_redToAvgRatio;
};

} // namespace Digikam

#endif // DIGIKAM_RED_EYE_CORRECTION_CONTAINER_H
